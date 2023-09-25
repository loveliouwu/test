## FolderListModel   

### 可用于model/delegate代理中的属性  
- fileNmae
- filePath
- fileURL
- fileUrl
- fileBaseName
- fileSuffix
- fileSize
- fileModified 
- fileAccessed 
- fileIsDir  

### 

```c++
import QtQuick 2.0
import QtQuick.Controls 2.5
import Qt.labs.folderlistmodel 2.2
Item {
    ListView{
        width: 200
        height: 400

        FolderListModel{
            id: id_folderListModel
            folder: "."
            showDirs: true
            showDirsFirst: true
            showDotAndDotDot: true
            showFiles: true
            showHidden: true
            sortField: FolderListModel.Type

            Component.onCompleted: {
                console.log("folder :" + folder)
            }
        }

        Component{
            id: id_fileDelegate
            Text{
                height: 20
                width: 200
                text: fileName
                font.pixelSize: 14
                color: fileIsDir ? "green" : "blue"

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        color = "pink"
                        console.log("filePath :" + filePath)
                        console.log("fileURL :" + fileURL)
                        console.log("fileUrl :" + fileUrl)
                        console.log("fileBaseName :" + fileBaseName)
                        console.log("fileSize :" + fileSize)
                    }
                    onExited: {
                        color = fileIsDir ? "green" : "blue"
                    }
                    onDoubleClicked: {
                        if(fileName === ".."){
                            id_folderListModel.folder = id_folderListModel.parentFolder
                            return
                        }
                        if(fileIsDir){
                            id_folderListModel.folder = fileUrl
                            console.log("double click fileName:"+ filePath + ";  folder:" + id_folderListModel.folder)
                        }
                    }
                }
            }
        }

        model: id_folderListModel
        delegate: id_fileDelegate

    }


}




```