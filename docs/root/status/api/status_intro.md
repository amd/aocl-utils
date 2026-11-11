<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# Status Integration for C++ Libraries

This library offers a straightforward way to incorporate status messages into a C++ library or application.

## Status Workflow

1. Derive a custom error class from `ErrorBase`; `GenericError` is a `final`
   convenience implementation and cannot be subclassed.
2. Implement the required `IError` operations, including the error message,
   error state, and module identifier.
3. Provide creator functions returning `IError` references that wrap the
   custom error class.
4. Define custom status functions in a dedicated source file.

For a complete example, refer to:

• `Library/Tests/Status/Tests/CalculatorStatus.cc`

• `Library/Tests/Status/Include/CalculatorStatus.hh`

• `Library/Tests/Status/Lib/CalculatorError.cc`

• `Library/Tests/Status/Include/CalculatorError.hh`


These files demonstrate a simple calculator library illustrating error handling and status reporting.
