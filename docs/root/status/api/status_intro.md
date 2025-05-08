# Status Integration for C++ Libraries

This library offers a straightforward way to incorporate status messages into a C++ library or application.

## Status Workflow

1. Derive a custom Error class from `GenericError`.
2. Provide functions returning `IError`, wrapping the custom Error class.
3. Define custom status functions in a dedicated source file.
4. Maintain status messages in the custom Error class and its associated functions.

For a complete example, refer to:

• Lib/CalculatorStatus.cc

• Include/CalculatorStatus.hh

• Lib/CalculatorError.cc

• Include/CalculatorError.hh


These files demonstrate a simple calculator library illustrating error handling and status reporting.
