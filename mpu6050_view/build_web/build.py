import os
import shutil
from collections import deque
Import("env")

""" 
执行npm 打包Web 项目
并复制到 data目录用于构建文件系统
"""


def run_npm(*args, **kwargs):
    print("build web")
    os.system("cd web && npm install strip-ansi && npm run build >build.log")

    if os.path.exists("./data"):
        print("remove data")
        shutil.rmtree("./data")

    os.mkdir("./data")
    for e in os.listdir("./web/dist/"):
        src = "./web/dist/" + e
        print("copy  " + src)
        if os.path.isdir(src):
            shutil.copytree(src, "./data/" + e)
        else:
            shutil.copy(src, "./data/" + e)

    web_index(*args, **kwargs)

def web_index(*args, **kwargs):
    queue = deque()
    # Use a breakpoint in the code line below to debug your script.
    data = "./data"
    queue.append("")
    with open('./src/index.h', 'w', encoding='utf-8') as file:
        file.write("""// Automatically generated by build_web/build.py, do not edit
#ifndef MPU6050_VIEW_INDEX_H
#define MPU6050_VIEW_INDEX_H

#include <Arduino.h>

const char* const webSrc[] = {
""")
        while queue:
            element = queue.popleft()
            if os.path.isdir(data + element):
                for child in os.listdir(data + element):
                    queue.append(element + "/" + child)
            else:
                file.write("        \"")
                file.write(element[:-3])
                file.write("\"")
                if queue:
                    file.write(",\n")
                else:
                    file.write("\n")

        file.write("""};
#endif // MPU6050_VIEW_INDEX_H""")

env.AddCustomTarget(
    name="buildWeb",
    dependencies=None,
    actions=[run_npm],
    title="Build Web",
    description="build web"
)

env.AddCustomTarget(
    name="webIndex",
    dependencies=None,
    actions=[web_index],
    title="Generate Web Index",
    description="Generate src/index.h"
)