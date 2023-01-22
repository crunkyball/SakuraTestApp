#include "TestAppGame.h"

using namespace Sakura;

int main(int argc, char** argv)
{
    SaGameProperties properties;
    properties.m_name = "Sakura Test App";
    properties.m_xResolution = 1280;
    properties.m_yResolution = 720;

    TestAppGame testGame(properties);

    testGame.Run();

    return 0;
}
