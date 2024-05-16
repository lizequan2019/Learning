import QtQuick 2.0
import "./package-compontent"

Item {
    id:root
    width:500
    height:500

    Cell2{
        id:rect1
        cellColor:"#ff0000"
        width: 50
        height:50
        anchors.left: parent.left
        state:"none"

        states: [//可以存放多个状态
            State {
                name: "state_left-to-right"
                AnchorChanges { target: rect1
                                anchors.left:undefined //先释放左边锚点,再确定右边锚点
                                anchors.right:parent.right
                              }
            },
            State {
                name: "state_right-to-left"
                AnchorChanges { target: rect1
                                anchors.right:undefined //先释放右边锚点,再确定右边锚点
                                anchors.left:parent.left
                              }
            }
        ]

        transitions: [
            Transition {
                from: "none"
                to: "state_left-to-right"
                AnchorAnimation {
                    duration: 100 //锚点动画持续时间
                }
            },
            Transition {
                from: "state_left-to-right"
                to: "state_right-to-left"
                AnchorAnimation {
                    duration: 1000 //锚点动画持续时间
                }
            },
            Transition {
                from: "state_right-to-left"
                to: "state_left-to-right"
                AnchorAnimation {
                    duration: 100 //锚点动画持续时间
                }
            }
        ]

        MouseArea { //鼠标区域
            anchors.fill: parent
            onClicked: { //点击槽函数 可以写写逻辑

                if(rect1.state == "none"){
                    rect1.state = "state_left-to-right"
                }else if(rect1.state == "state_left-to-right"){
                    rect1.state = "state_right-to-left"
                }else if(rect1.state == "state_right-to-left"){
                    rect1.state = "state_left-to-right"
                }
            }
        }
    }

}
