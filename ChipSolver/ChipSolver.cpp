#include "ChipSolver.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <vector>
#include <ctime>
#include "CodeX/CodeX.h"
#include <cstdlib>
#include <QSettings>
#include <QFile>

constexpr auto QrcBase = ":/ChipSolver/Resources/";

ChipSolver::ChipSolver(QObject* parent):
	QThread(parent),
	useEquipped_(false),
	useLocked_(false),
	useAlt_(false),
	running_(false)
{
	Q_INIT_RESOURCE(ChipSolver);

	qRegisterMetaType<TargetBlock>("TargetBlock");
	
	QFile file(QString(QrcBase) + "squads.json");
	file.open(QIODevice::ReadOnly);
	configInfo_ = QJsonDocument::fromJson(file.readAll()).object();
	file.close();
	ChipConfig::initConfig();
	for(const auto& squad : configInfo_.keys())
	{
		file.setFileName(QrcBase + squad + ".json");
		file.open(QIODevice::ReadOnly);
		auto obj = QJsonDocument::fromJson(file.readAll()).object();
		file.close();
		
		// 读入格子信息
		ChipViewInfo info;
		info.width = obj["width"].toInt();
		info.height = obj["height"].toInt();
		auto map = obj["map"].toArray();
		for(auto i = 0;i < map.size();++i)
		{
			info.map.emplace_back();
			for(const auto& it : map[i].toString().toLatin1())
			{
				info.map[i].push_back(-(it - '0'));
			}
		}
		squadView_[squad] = info;

		// 读取芯片拼图方案
		const auto& configFile = configInfo_[squad].toObject();
		for(const auto& name : configFile.keys())
		{
			SquadConfig squadConfig;
			// 重装的基本信息，实际上和方案无关，但是没地方存了，懒得单独再存
			squadConfig.blocks = obj["blocks"].toInt();
			squadConfig.optional = obj["optional"].toObject()[name].toInt();
			auto maxBlocks = obj["MaxBlocks"].toObject();
			squadConfig.maxValue.damageBlock = maxBlocks["damage"].toInt();
			squadConfig.maxValue.defbreakBlock = maxBlocks["def_break"].toInt();
			squadConfig.maxValue.hitBlock = maxBlocks["hit"].toInt();
			squadConfig.maxValue.reloadBlock = maxBlocks["reload"].toInt();
			auto maxValues = obj["MaxValues"].toObject();
			squadConfig.maxValue.damageValue = maxValues["damage"].toInt();
			squadConfig.maxValue.defbreakValue = maxValues["def_break"].toInt();
			squadConfig.maxValue.hitValue = maxValues["hit"].toInt();
			squadConfig.maxValue.reloadValue = maxValues["reload"].toInt();
			if (maxBlocks.contains("free"))
			{
				squadConfig.maxValue.squad = maxBlocks["free"].toInt();
			}
			else
			{
				squadConfig.maxValue.squad = 0;
			}
			squadConfig.color = obj["color"].toInt();
			squadConfig.palindrome = obj["palindrome"].toInt();
			
			file.setFileName(QrcBase + configFile[name].toString());
			file.open(QIODevice::ReadOnly);
			auto configArray = QJsonDocument::fromJson(file.readAll()).array();
			file.close();
			for(const auto& it : configArray)
			{
				Config config;
				for(const auto& c : it.toArray())
				{
					config.push_back(ChipPuzzleOption(c.toObject()));
				}
				squadConfig.configs.push_back(config);
			}
			// name是方案名
			configs_[squad][name] = squadConfig;
			maxValues_[squad] = squadConfig.maxValue;
		}
	}
}

QStringList ChipSolver::squadList() const
{
	return configInfo_.keys();
}

QStringList ChipSolver::configList(const QString& squad)
{
	targetSquadName_ = squad;
	return configInfo_[squad].toObject().keys();
}

GFChip ChipSolver::squadMaxValue(const QString& squad)
{
	return maxValues_[squad];
}

void ChipSolver::setTargetBlock(const TargetBlock& block)
{
	targetBlock_ = block;
}

void ChipSolver::setTargetConfig(const QString& name)
{
	targetConfigName_ = name;
}

void ChipSolver::setUseEquipped(bool b)
{
	useEquipped_ = b;
}

void ChipSolver::setUseLocked(bool b)
{
	useLocked_ = b;
}

void ChipSolver::setUseAlt(bool b)
{
	useAlt_ = b;
}

void ChipSolver::stop()
{
	running_ = false;
	for (auto th : threads_)
	{
		th->quit();
	}
}

ChipViewInfo ChipSolver::solution2ChipView(const Solution& solution, const QString& squad)
{
	auto view = squadView_[targetSquadName_];
	if(squad != "")
	{
		view = squadView_[squad];
	}
	for(auto i = 0;i < solution.chips.size();++i)
	{
		const auto& chip = solution.chips[i];
		const auto& chipConfig = ChipConfig::getConfig(CodeX::instance()->chips[chip.no].gridID).rotate90(chip.rotate);
		for(auto x = 0;x < chipConfig.width;++x)
		{
			for(auto y = 0;y < chipConfig.height;++y)
			{
				if(chipConfig.map[y][x] == '1')
				{
					view.map[y + chip.y][x + chip.x] = i + 1;
				}
			}
		}
	}
	typedef std::vector<std::vector<int>> Map;
	auto rotate = [](const Map& map) -> Map
	{
		Map r;
		std::vector<int> row;
		row.resize(map.size(), 0);
		r.resize(map[0].size(), row);
		for (auto i = 0u; i < r.size(); ++i)
		{
			for (auto j = 0u; j < r[i].size(); ++j)
			{
				r[i][j] = map[map.size() - j - 1][i];
			}
		}
		return r;
	};
	for(auto i = 0;i < solution.totalValue.rotate;++i)
	{
		view.map = rotate(view.map);
	}
	return view;
}

void ChipSolver::run()
{
	running_ = true;
	tmpSquadConfig_ = configs_[targetSquadName_][targetConfigName_];
	lastSolveNumber_ = 0;
	tmpSolutionNumber_ = 0;
	tmpTarget_ = targetBlock_;
	tmpTarget_.error += tmpSquadConfig_.optional; // 附加额外的可空格
	percent = 0;
	configIndex_ = 0;
	solutions.clear();
	tmpChips_.resize(8, 0);
	thSolutionQueue_ = std::queue<std::shared_ptr<priority_queue<Solution>>>();
	emit solvePercentChanged(0);
	chipUsedFunc = CodeX::instance()->getChipUsedFunc();

	// 对153特殊处理
	if(targetSquadName_ == "Mk-153")
	{
		if(targetBlock_.damageBlock == 17 && targetBlock_.defbreakBlock == 9 && targetBlock_.hitBlock == 7 && targetBlock_.reloadBlock == 5)
		{
			tmpSquadConfig_.maxValue.damageValue = 195;
			tmpSquadConfig_.maxValue.defbreakValue = 273;
			tmpSquadConfig_.maxValue.hitValue = 140;
			tmpSquadConfig_.maxValue.reloadValue = 64;
		}
		else if (targetBlock_.damageBlock == 17 && targetBlock_.defbreakBlock == 8 && targetBlock_.hitBlock == 8 && targetBlock_.reloadBlock == 5)
		{
			tmpSquadConfig_.maxValue.damageValue = 195;
			tmpSquadConfig_.maxValue.defbreakValue = 263;
			tmpSquadConfig_.maxValue.hitValue = 158;
			tmpSquadConfig_.maxValue.reloadValue = 64;
		}
		else if (targetBlock_.damageBlock == 16 && targetBlock_.defbreakBlock == 8 && targetBlock_.hitBlock == 9 && targetBlock_.reloadBlock == 5)
		{
			tmpSquadConfig_.maxValue.damageValue = 189;
			tmpSquadConfig_.maxValue.defbreakValue = 263;
			tmpSquadConfig_.maxValue.hitValue = 176;
			tmpSquadConfig_.maxValue.reloadValue = 64;
		}
	}
	QSettings settings;
	solveRunning_ = true;
	
	threads_.resize(settings.value("/Sys/Threads", 1).toInt(), nullptr);
	for (auto& th : threads_)
	{
		th = QThread::create(&ChipSolver::startSolve, this);
		connect(th, &QThread::finished, th, &QThread::deleteLater);
		th->start();
	}
	auto mergeTh = QThread::create(&ChipSolver::merge, this);
	mergeTh->start();
	for (auto th : threads_)
		th->wait();
	// 等待合并结果
	while (!thSolutionQueue_.empty())
	{
		QThread::msleep(1);
	}
	solveRunning_ = false;
	mergeTh->wait();
	delete mergeTh;
	running_ = false;
	
	emit solvePercentChanged(100);
}

bool ChipSolver::satisfyConfig(const Config& config)
{
	QMap<int, int> require;
	bool ans = true;
	for(const auto& it : config)
	{
		++require[it.no];
	}
	for(const auto& it : require.keys())
	{
		ans = ans && (CodeX::instance()->gridChips[tmpSquadConfig_.color][it].size() >= require[it]);
	}
	return ans;
}

bool ChipSolver::checkOverflow(const TargetBlock& target, const GFChip& chips, const GFChip& add) const
{
	int over = 0;
	over += std::max(0, chips.defbreakBlock + add.defbreakBlock - target.defbreakBlock);
	if (over > target.error)
		return true;
	over += std::max(0, chips.damageBlock + add.damageBlock - target.damageBlock);
	if (over > target.error)
		return true;
	over += std::max(0, chips.reloadBlock + add.reloadBlock - target.reloadBlock);
	if (over > target.error)
		return true;
	over += std::max(0, chips.hitBlock + add.hitBlock - target.hitBlock);
	return over > target.error;
}

void ChipSolver::findSolution(SolverParam& param)
{
	if (!running_)
		return;
	if (param.k >= param.config.size())
	{
		if (param.solutionSet.count(param.curChips) > 0)
			return;
		param.solutionSet.insert(param.curChips);
		
		param.solution.chips.resize(param.k);
		param.solution.totalValue.no = 0;
		param.solution.totalValue.id = 0;
		param.solution.totalValue.exp = 0;
		param.solution.totalValue.rotate = 0;
		for (const auto& it : param.solution.chips)
		{
			const auto& chip = CodeX::instance()->chips[it.no];
			auto r = chip.rotate % ChipConfig::getConfig(chip.gridID).direction;
			param.solution.totalValue.no += int(it.rotate != r);
			param.solution.totalValue.exp += chip.exp;
		}
		if (tmpSquadConfig_.palindrome > 0)
		{
			// 枚举整体旋转方向
			for (auto i = tmpSquadConfig_.palindrome; i < 4; i += tmpSquadConfig_.palindrome)
			{
				int sum = 0;
				// 对每个芯片附加一次旋转
				for (const auto& it : param.solution.chips)
				{
					const auto& chip = CodeX::instance()->chips[it.no];
					auto r = chip.rotate + i;
					r %= ChipConfig::getConfig(chip.gridID).direction; // 考虑芯片自身对称问题
					sum += int(r != it.rotate);
				}
				if (sum < param.solution.totalValue.no)
				{
					param.solution.totalValue.no = sum;
					param.solution.totalValue.rotate = i;
				}
			}
		}
		param.solution.totalValue.id += std::min(0, param.solution.totalValue.defbreakValue - tmpSquadConfig_.maxValue.defbreakValue);
		param.solution.totalValue.id += std::min(0, param.solution.totalValue.damageValue - tmpSquadConfig_.maxValue.damageValue);
		param.solution.totalValue.id += std::min(0, param.solution.totalValue.reloadValue - tmpSquadConfig_.maxValue.reloadValue);
		param.solution.totalValue.id += std::min(0, param.solution.totalValue.hitValue - tmpSquadConfig_.maxValue.hitValue);

		param.queue->push(param.solution);
		if (param.queue->size() > targetBlock_.showNumber)
			param.queue->pop();
		
		++tmpSolutionNumber_;

		if (tmpSolutionNumber_ - lastSolveNumber_ >= 10000)
		{
			lastSolveNumber_.store(tmpSolutionNumber_);
			emit solveNumberChanged(tmpSolutionNumber_);
			if (targetBlock_.maxNumber >= 0 && tmpSolutionNumber_ > targetBlock_.maxNumber)
			{
				running_ = false;
			}
		}

		return;
	}

	//获取当前所需型号的芯片列表
	auto& chips = param.gridChips[tmpSquadConfig_.color][param.config[param.k].no];
	// 先保存这一步的芯片配置，后续更新id
	param.solution.chips[param.k] = param.config[param.k];
	for (auto& chip : chips)
	{
		if (!running_)
			return;
		if (chip.squad & 0x8000) //已使用
			continue;
		if ((chip.locked && !useLocked_)// 已锁定且不使用已锁定
			|| (chip.squad && !useEquipped_))// 已装备且不使用已装备
		{
			continue;
		}
		// 已使用且不使用已装备
		if (!useAlt_ && chipUsedFunc(chip.no))
		{
			continue;
		}
		// 溢出了不满足要求
		if (checkOverflow(tmpTarget_, param.solution.totalValue, chip))
		{
			continue;
		}
		chip.squad |= 0x8000; //符号位置1，反正小队号都是正数
		param.solution.totalValue += chip;
		param.solution.chips[param.k].no = chip.no; // 更新id
		param.curChips[param.k] = chip.no;
		++param.k;
		findSolution(param);
		--param.k;
		chip.squad &= ~0x8000; // 符号位置0
		param.solution.totalValue -= chip;
	}
}

void ChipSolver::startSolve()
{
	SolverParam param;
	param.chips = CodeX::instance()->chips;
	param.gridChips = CodeX::instance()->gridChips;

	while (running_)
	{
		int index =
			index = ++configIndex_;
		// 取出本线程轮到的配置序号
		if (index >= tmpSquadConfig_.configs.size()) // 结束当前线程
			return;

		if (!satisfyConfig(tmpSquadConfig_.configs[index]))
		{
			continue;
		}

		param.config = tmpSquadConfig_.configs[index];
		param.solutionSet = decltype(param.solutionSet)();
		param.k = 0;
		param.curChips.resize(8, 0);
		param.solution.chips.resize(8, ChipPuzzleOption());
		param.queue.reset(new priority_queue<Solution>());

		findSolution(param);

		// 放到队列里交给合并线程处理
		{
			std::unique_lock<std::mutex> locker(queueMutex_);
			thSolutionQueue_.push(param.queue);
		}
		queueCV_.notify_all();

		// 计算当前进度百分比
		int per = round((index + 1) * 100.0 / tmpSquadConfig_.configs.size());
		if (per > percent)
		{
			percent = per;
			emit solvePercentChanged(percent);
		}

		lastSolveNumber_.store(tmpSolutionNumber_);
		emit solveNumberChanged(tmpSolutionNumber_);
		if (targetBlock_.maxNumber > 0 && tmpSolutionNumber_ >= targetBlock_.maxNumber)
		{
			running_ = false;
			break;
		}
	}
}

void ChipSolver::merge()
{
	do
	{
		{
			std::unique_lock<std::mutex> locker(queueMutex_);
			queueCV_.wait_for(locker, std::chrono::milliseconds(10)/*, [&]() {return !thSolutionQueue_.empty() || !solveRunning_; }*/);
		}

		bool add = false;
		std::unique_lock<std::mutex> locker(queueMutex_);
		while (!thSolutionQueue_.empty())
		{
			auto it = thSolutionQueue_.front();
			thSolutionQueue_.pop();
			solutions.insert(solutions.end(), it->getContainer().begin(), it->getContainer().end());
			add = true;
		}
		if (add)
		{
			std::sort(solutions.begin(), solutions.end());
			if (solutions.size() > targetBlock_.showNumber)
				solutions.resize(targetBlock_.showNumber);
		}
	} while (solveRunning_);
}
