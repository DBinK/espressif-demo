import os
import shutil

Import("env")

npm = env.GetProjectOption("npm")

""" 
执行npm 打包vue 项目
并复制到 data目录用于构建文件系统
"""


def run_npm(*args, **kwargs):
    print("build web")
    os.system("cd web && {npm} install strip-ansi && {npm} run build >build.log".format(npm=npm))

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


env.AddCustomTarget(
    name="buildWeb",
    dependencies=None,
    actions=[run_npm],
    title="Build Vue",
    description="build web"
)
