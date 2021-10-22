from PySide6.QtGui import QAction

def changeZoom(mode):
    size = mainWindow.font().pixelSize()
    text_edit = mainWindow.getTextEdit()

    size = size + 1 if mode == "+" else size - 1
    mainWindow.setStyleSheet(f"font-size: {size}px")
    text_edit.setStyleSheet(f"font-size: {size}px")

zoom_out = QAction("Zoom Out")
zoom_out.triggered.connect(lambda: changeZoom("-"))

zoom_in = QAction("Zoom In")
zoom_in.triggered.connect(lambda: changeZoom("+"))

tool_bar = mainWindow.getToolBar()
tool_bar.addAction(zoom_in)
tool_bar.addAction(zoom_out)
