#include "Application.h"

using namespace Bamtang;

int main(int argc, char* argv[])
{
    Application* w = new Application("LABERINTO 3D","33", 1080, 720);

    w->Render();
    w->Clear();

    return 0;
}

