#include "palette.h"
#include "util.h"

std::filesystem::path PaletteAsset::generateAssetPath() {
    std::filesystem::path asset_path = path;
    return asset_path.replace_extension(".pal");
}

void PaletteAsset::convertToHumanReadable(const std::vector<char>& baserom) {
    (void)baserom;

    std::vector<std::string> cmd;
    cmd.push_back(tool_path("gbagfx"));
    cmd.push_back(path.string());
    cmd.push_back(assetPath.string());
    check_call(cmd);
}

void PaletteAsset::buildToBinary() {
    std::vector<std::string> cmd;
    cmd.push_back(tool_path("gbagfx"));
    cmd.push_back(assetPath.string());
    cmd.push_back(path.string());
    check_call(cmd);
}