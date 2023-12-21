#include <gtest/gtest.h>
#include <filesystem>
#include <Python.h>
#include <string>
#include <vector>
#include <map>
namespace
{
class QemuTest : public ::testing::TestWithParam<std::tuple<std::string, std::vector<bool>>> {
    private:
        int callQemuEmulator(const char* cpu, const char* testname)
        {
            int ret = 0;

            Py_Initialize();
            if (!Py_IsInitialized()) {
                std::cerr << "Failed to initialize the Python interpreter";
            }
            /* Update PYTHONPATH with the path of the python module to be loaded*/
            std::string src_path = PROJECT_SOURCE_DIR;
            src_path += "/Library/Tests/Cpuid/";
            PyObject* sys = PyImport_ImportModule( "sys" );
            PyObject* sys_path = PyObject_GetAttrString( sys, "path" );
            PyObject* folder_path = PyUnicode_FromString( src_path.c_str() );
            PyList_Append( sys_path, folder_path );

            /*Load The test module */
            PyObject *pModule =  PyImport_ImportModule("CpuidTest");

            if (pModule != NULL) {
                PyObject *pFunc = PyObject_GetAttrString(pModule, "run_qemu");

                if (pFunc != NULL && PyCallable_Check(pFunc)) {
                    /* Call the run_qemu function with arguments */
                    std::string binary_path = PROJECT_BUILD_DIR;
                    binary_path+= "/Release/core_CpuidTest";

                    PyObject *pArgs = PyTuple_Pack(4, PyUnicode_DecodeFSDefault("x86_64"),
                            PyUnicode_DecodeFSDefault(cpu),
                            PyUnicode_DecodeFSDefault(binary_path.c_str()),
                            PyUnicode_DecodeFSDefault(testname));
                    PyObject *pValue = PyObject_CallObject(pFunc, pArgs);

                    if (pValue != NULL) {
                        ret = PyLong_AsLong(pValue);
                    }
                }
            }
            /* Finalize the Python interpreter */
            Py_Finalize();

            return ret;
        }
    protected:
        std::vector<bool> testAll(const std::string& cpuType, const std::vector<std::string>& testNames) {
            std::vector<bool> results;
            for (auto testName : testNames) {
                bool result = callQemuEmulator(cpuType.c_str(), testName.c_str());
                results.push_back(result);
            }
            return results;
        }
         void TearDown() override {
            std::string clean_path = PROJECT_SOURCE_DIR;
            clean_path += "/Library/Tests/Cpuid/__pycache__/";
            std::filesystem::remove_all(clean_path);
    }
};

const std::vector<std::tuple<std::string, std::vector<bool>>> testParameters = {
    /*The expected value of x86_64v4 is intetionally set to false, as it is not
     * currently supported on qemu
     */
    {"EPYC-Genoa-v1", {true, false, true, true, false}},
    {"EPYC-Milan-v1", {true, false, true, true, false}},
    {"EPYC-Milan-v2", {true, false, true, true, false}},
    {"EPYC-Rome-v1", {true, false, true, true, false}},
    {"EPYC-Rome-v2", {true, false, true, true, false}},
    {"EPYC-Rome-v3", {true, false, true, true, false}},
    {"EPYC-Rome-v4", {true, false, true, true, false}},
    {"EPYC-v1", {true, false, true, true, false}},
    {"EPYC-v2", {true, false, true, true, false}},
    {"EPYC-v3", {true, false, true, true, false}},
    {"EPYC-v4", {true, false, true, true, false}},
    {"Opteron_G1-v1", {true, false, false, false, false}},
    {"Opteron_G2-v1", {true, false, false, false, false}},
    {"Opteron_G3-v1", {true, false, false, false, false}},
    {"Opteron_G4-v1", {true, false, true, false, false}},
    {"Opteron_G5-v1", {true, false, true, false, false}},
    {"phenom-v1", {true, false, false, false, false}},
    {"Broadwell-v1", {false, true, true, true, false}},
    {"Denverton-v1", {false, true, true, false, false}},
    {"Conroe-v1", {false, true, false, false, false}},
    {"Skylake-Server-v1", {false, true, true, true, true}}
};

INSTANTIATE_TEST_SUITE_P(QemuTestSuite, QemuTest, ::testing::ValuesIn(testParameters));

TEST_P(QemuTest, CpuTypeTest) {
    const auto params = GetParam();
    const std::string cpuType = std::get<0>(params);
    const std::vector<std::string> testNames = {
        "X86Cpuid.DISABLED_isAMD",
        "X86Cpuid.DISABLED_isIntel",
        "X86Cpuid.DISABLED_isX86_64v2",
        "X86Cpuid.DISABLED_isX86_64v3",
        "X86Cpuid.DISABLED_isX86_64v4"
    };
    const std::vector<bool> expectedResults = std::get<1>(params);

    auto results = testAll(cpuType, testNames);
    EXPECT_EQ(results, expectedResults);

}
} // namespace
