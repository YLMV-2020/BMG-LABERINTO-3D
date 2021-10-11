#include "Application.h"

using namespace Bamtang;

int main(int argc, char* argv[])
{
    Application* application = new Application("LABERINTO 3D","33", 1080, 720);
    application->Render3D();
    return 0;
}

