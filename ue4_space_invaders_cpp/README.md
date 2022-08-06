# ue4-space-invaders-cpp
Space Invaders Game made in Unreal Engine 4 using C++

### NOTE:
In case you experience a `CompilerResultsLog     c1xx: fatal error C1076: compiler limit: internal heap limit reached`
error or something comparable follow the instructions in the docs.

You can copy the `UnrealBuildTool` folder from the git root into your directory of choice (I used `%AppData%\Roaming\Unreal Engine\UnrealBuildTool`).

- https://forums.unrealengine.com/t/compiler-limit-internal-heap-limit-reached/151184/2
- https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/BuildTools/UnrealBuildTool/BuildConfiguration/


Also maxing out the virtual memory (Pagefile) file is helpful (which I did as well).
- https://www.tomshardware.com/news/how-to-manage-virtual-memory-pagefile-windows-10,36929.html

![](https://user-images.githubusercontent.com/1466920/55278725-2c571f00-5310-11e9-922e-cebc4f5a8496.png)
