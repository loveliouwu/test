添加sshkey到git
    1.生成key
        ssh-keygen -t rsa -C "youremail@example.com"
    2.添加到github
    3.github上或者本地建一个仓库
        本地和github仓库关联
        git remote add origin 仓库地址
        当你是克隆远程仓库时，实际上自动把本地的master分支和远程关联在一起

创建合并分支
    1.查看当前分支
        git branch
    2.创建新分支
        git checkout -b new_branch
            -b 表示创建并切换。
            相当于
            git branch new_branch
            git checkout new_branch
    3.合并分支
        git merge new_branch
        合并指定分支到当前分支，
    4.删除分支
        git branch -d new_branch
    
        
branch和checkout的使用：
    branch:
        一般用于分支的操作，比如创建分支，查看分支等
        1.git branch
        2.git branch -r
            查看远程版本库分支列表
        3.git branch -a
            查看所有分支列表，包括本地和远程
        4.git branch dev
            创建名为dev的分支
        5.git branch -d dev 
            删除分支 -D 当分支合并出错时D
        6.git branch -m oldname newname
    
    checkout:
        一般用来操作文件或者操作分支
        操作文件：
            1.git checkout filename 
                放弃单个文件的修改
            2.git checkout .
                放弃当前目录下的修改
            3.git checkout master 
                切换到主分支
            4.git checkout -b new_dev
                如果分支存在则只切换分支，不存在创建并切换到分支
            
stash ：
    保存当前工作区内容，不提交仍旧是干净的工作区
    1.git stash
        保存本地修改
    2.git stash list
        列出保存的工作现场
    3.git stash apply stash@{0}
        恢复现场 ,但不删除保存的现场文件
    4.git stash pop  stash@{0}
        恢复现场，并删除现场文件


 git tag 打标签：
    1.创建标签
        git tag v1.0
        对当前的版本打上标签
    2.git tag 查看标签
    3.git tar v0.9 f53633  对过去版本添加标签
    4.git show v0.9 查看标签信息
    5.git tag -a v0.1 -m "version 0.1 msg" 1094adb
        创建标签并添加文字说明　－ａ指定标签名