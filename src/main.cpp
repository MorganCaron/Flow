import std;
import CppUtils;
import Flow.Model.UserRequestLog;
import Flow.UI.FlowTerminalUI;

auto start([[maybe_unused]] std::span<const std::string_view> arguments) -> int
{
	auto isWatchMode = false;
	auto currentLogLevel = Flow::UI::LogLevel::Info;

	for (auto argumentIndex = 1uz; argumentIndex < std::ranges::size(arguments); ++argumentIndex)
	{
		const auto argument = arguments[argumentIndex];
		if (argument == "--watch")
			isWatchMode = true;
		else if (argument == "--quiet")
			currentLogLevel = Flow::UI::LogLevel::Off;
		else if (argument.starts_with("--log-level="))
			currentLogLevel = Flow::UI::LogLevelMapping::toLhs(argument.substr(12), Flow::UI::LogLevel::Info);
		else if (argument == "--log-level" and argumentIndex + 1 < std::ranges::size(arguments))
			currentLogLevel = Flow::UI::LogLevelMapping::toLhs(arguments[++argumentIndex], Flow::UI::LogLevel::Info);
	}

	Flow::UI::setLogLevel(currentLogLevel);
	Flow::UI::initTerminalUI();
	Flow::UI::printBanner();

	try
	{
		using namespace CppUtils::Memory::Literals;

		std::filesystem::create_directories("logs");
		auto fileOptions = CppUtils::LogRotateOptions{
			.filePath = "logs/flow.log",
			.maxFiles = 5,
			.maxSize = 10_MiB};
		auto fileSink = [options = fileOptions](const std::string& message) mutable {
			CppUtils::logRotate(options);
			CppUtils::FileSystem::String::append(options.filePath, std::format("[{}] {}\n", Flow::UI::getFormattedDateTime(), message));
		};
		auto& eventQueue = CppUtils::Logger<"Flow">::eventQueue();
		eventQueue.subscribe(fileSink);
		eventQueue.subscribe<"detail">(fileSink);
		eventQueue.subscribe<"debug">(fileSink);
		eventQueue.subscribe<"info">(fileSink);
		eventQueue.subscribe<"warning">(fileSink);
		eventQueue.subscribe<"error">(fileSink);
		eventQueue.subscribe<"success">(fileSink);

		auto threadPool = CppUtils::Thread::ThreadPool{};
		CppUtils::Logger<"Flow">::template print<"info">("ThreadPool initialized with {} worker threads (LogLevel: {})", threadPool.numberThreads(), currentLogLevel);

		auto runner = CppUtils::Language::Xml::FlowRunner{threadPool, "data/flows"};
		runner.registerMapping<Flow::Model::UserRequestLog, Flow::Mapping::WebLogMapping>("web");
		runner.registerMapping<Flow::Model::UserRequestLog, Flow::Mapping::ApiLogMapping>("api");
		runner.registerMapping<Flow::Model::UserRequestLog, Flow::Mapping::StandardLogMapping>("standard");
		runner.registerMapping<Flow::Model::UserRequestLog, Flow::Mapping::StandardLogMapping>("");
		runner.loadFlows();

		if (isWatchMode)
		{
			runner.startWatching();
			CppUtils::Logger<"Flow">::template print<"info">("Running in Watch Mode. Press Enter to stop...");
			auto line = std::string{};
			std::getline(std::cin, line);
			runner.stopWatching();
		}
		else
		{
			const auto stats = runner.execute();
			Flow::UI::printSummaryCard(
				stats,
				std::ranges::size(runner.flows().value()));
		}

		CppUtils::Logger<"Flow">::waitUntilFinished();
	}
	catch (const std::exception& exception)
	{
		CppUtils::Logger<"Flow">::template print<"error">("Fatal Flow Exception: {}", exception.what());
		CppUtils::Logger<"Flow">::waitUntilFinished();
		return CppUtils::exitFailure;
	}

	return CppUtils::exitSuccess;
}

auto main(const int argc, const char* argv[]) -> int
{
	CppUtils::System::installCrashHandler();
	CppUtils::Terminal::setConsoleOutputUTF8();
	return start(std::vector<std::string_view>{argv, argv + argc});
}
