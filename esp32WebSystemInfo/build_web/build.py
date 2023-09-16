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
    # 这一句的输出 在platformio的环境下 中文windows系统会遇到 UnicodeEncodeError: 'gbk' codec can't encode character 这个异常
    # 但不会使主线程失败 后续内容可以运行
    os.system("cd web && {npm} install && {npm} run build".format(npm=npm))

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
