# Introduction to logging using C++ API

This library provides a flexible logging system. Below are the main classes:

1. `Au::Logger::LogWriter`
   - Runs a dedicated thread for log processing.
   - Spools and commits messages to the chosen output.
   - Implemented as a singleton, use `Au::Logger::LogWriter::getLogWriter()`.

2. `Au::Logger::LogManager`
   - Manages log messages at a higher level.
   - Collects messages and forwards them to Au::Logger::LogWriter.
   - Your primary interface for logging in the application.

3. `Au::Logger::LoggerFactory`
   - Creates and configures loggers.
   - Offers ways to switch between different logger types such as console or file outputs.

4. `Au::Logger::Message`
   - Represents a single log entry with a `Timestamp` and a `Priority`.
   - Construct with a string payload and (optionally) a `Priority`.

5. `Au::Logger::ConsoleLogger`
   - Outputs log messages to the console.
   - Default choice for quick debugging.

6. `Au::Logger::FileLogger`
   - Writes log messages to a file.
   - Requires a filename during construction.

## Logger Workflow

A main logging thread is started by `Au::Logger::LogWriter`, which is responsible for collecting messages and writing them. Users typically interact with `Au::Logger::LogManager`, which forwards these logs to the global `Au::Logger::LogWriter` instance for final handling.

You can also decide where the logs should go by calling `Au::Logger::LogWriter::setLogger()`. Use `Au::Logger::LoggerFactory` to create custom outputs, like file-based or console-based loggers.

Finally, keep in mind that `Au::Logger::LogWriter` is a singleton. Call `Au::Logger::LogWriter::getLogWriter()` whenever you need to access its functionality.
