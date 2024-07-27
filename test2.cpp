#include "mars_logger.h"

void testBasicLogging() {
    // Test logging at different levels
    LogError("Error message");
    LogWarn("Warning message");
    LogInfo("Info message");
    LogDebug("Debug message");
    LogTrace("Trace message");
}

int main()
{
    testBasicLogging();
}
