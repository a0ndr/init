#include <SimpleIni.h>
#include <defines.h>
#include <gsl/assert>
#include <iostream>
#include <fmt/core.h>

using namespace std;

int main() {
    CSimpleIniA ini;
    ini.SetUnicode();

    SI_Error rc = ini.LoadFile(INIT_CONFIG_PATH);
    if (rc != SI_OK) {
        cerr << fmt::format("Could not load config file {} ({})", INIT_CONFIG_PATH, rc) << endl;
        return 1;
    }
    Expects(rc == SI_OK);
}
