from PySide6.QtGui import QAction

colorMode = "light"
mainWindowColor = {"dark": "background-color: #31363b; color: white;",
                   "light": "background-color: #e8e8e7; color: black;"}

textEditColor = {"dark": "background-color: gray;",
                 "light": "background-color: white;"}

def changeMode():
    global colorMode
    text_edit = mainWindow.getTextEdit()
    size = mainWindow.font().pixelSize()

    if colorMode == "dark":
        mainWindow.setStyleSheet(f"font-size: {size}px;{mainWindowColor['light']}")
        text_edit.setStyleSheet(f"font-size: {size}px;{textEditColor['light']}")
        colorMode = "light"
    else:
        mainWindow.setStyleSheet(f"font-size: {size}px;{mainWindowColor['dark']}")
        text_edit.setStyleSheet(f"font-size: {size}px;{textEditColor['dark']}")
        colorMode = "dark"

tool_bar = mainWindow.getToolBar()
action = QAction("Dark Mode")
tool_bar.addAction(action)
action.triggered.connect(changeMode)
