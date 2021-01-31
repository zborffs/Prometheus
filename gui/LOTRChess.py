from tkinter import *
from tkinter import scrolledtext
from PIL import Image, ImageTk
from PyLOTRChess import Board, BoardView
from tkinter.constants import INSERT, END


class App:
    def __init__(self, title, x, y):
        self.window_ = Tk()
        self.window_.title(title)
        geom_str = '{0}x{1}'.format(x, y)
        self.window_.geometry(geom_str)

        # add stuff
        self.board_ = Board('startpos')
        self.boardview_ = BoardView(self, self.board_)
        self.board_label = self._add_label(0, 0, self.boardview_.get(), self.boardview_.click_handler)
        self.engine_txt_window_ = scrolledtext.ScrolledText(self.window_, height=10, width=125)
        # self.engine_txt_window_.configure(state ='disabled')
        self.engine_txt_window_.place(x=0, y=850)
        self.button_ = self._add_button(400, 805, self.button_handler)

    def _add_label(self, x, y, img=None, handler=None):
        lbl = Label(self.window_, image=img)
        lbl.place(x=x, y=y)
        if handler is not None:
            lbl.bind("<Button-1>", handler)
        return lbl

    def _add_button(self, x, y, handler):
        btn = Button(self.window_, text="Click Me", command=handler)
        btn.place(x=x, y=y)
        return btn

    def _add_img(self, path):
        img = Image.open(path)
        return img

    def button_handler(self):
        stdout = "leasdf"
        self.engine_txt_window_.insert(END, stdout)

    def _add_tkimg(self, img):
        tkimg = ImageTk.PhotoImage(img)
        return tkimg

    def run(self):
        self.window_.mainloop()


if __name__ == '__main__':
    app = App("LOTR Chess", 900, 900)
    app.run()
