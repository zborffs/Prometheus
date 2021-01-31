import time
from PIL import Image, ImageTk, ImageOps
from .square import Square, square_to_ind
from .color import Color


class BoardView:
    def __init__(self, context, board):
        self.context_ = context

        # board image
        self.board_img_ = self._add_img("res/ChessBoard.png")

        # light
        self.light_bishop_ = self._add_img("res/Light_Bishop.png")
        self.light_king_ = self._add_img("res/Light_King.png")
        self.light_knight_ = self._add_img("res/Light_Knight.png")
        self.light_pawn_ = self._add_img("res/Light_Pawn.png")
        self.light_queen_ = self._add_img("res/Light_Queen.png")
        self.light_rook_ = self._add_img("res/Light_Rook.png")

        # dark
        self.dark_bishop_ = self._add_img("res/Dark_Bishop.png")
        self.dark_king_ = self._add_img("res/Dark_King.png")
        self.dark_knight_ = self._add_img("res/Dark_Knight.png")
        self.dark_pawn_ = self._add_img("res/Dark_Pawn.png")
        self.dark_queen_ = self._add_img("res/Dark_Queen.png")
        self.dark_rook_ = self._add_img("res/Dark_Rook.png")

        # gray circle
        self.gray_circle_ = self._add_img("res/Gray_Circle.png")

        # set the board up
        self.board_ = board
        self._draw_board()

        # tkimage member
        self.tk_board_ = ImageTk.PhotoImage(self.board_img_)

        # selected
        self.selected_ = Square.NoSquare

    def _algnot_to_pixel(self, algnot):
        # algnot: 'a1', 'b5', 'h7'
        x = (ord(algnot[0]) - ord('a')) * 100  # magic nums: 700 = board image width, 100 is piece widths
        y = 700 - ((ord(algnot[1]) - ord('1')) * 100)
        return x, y

    def _draw_board(self):
        board = self.board_.board()
        for i in range(0, len(board)):
            # i = 0 -> 1, i = 1 -> 2
            for j in range(0, len(board[i])):
                # j = 0 -> 'a', j = '1' -> 'b'
                algnot = chr(j + ord('a')) + chr(i + ord('1'))
                c = board[i][j]
                if c == 'r':
                    self.board_img_.paste(self.light_rook_, self._algnot_to_pixel(algnot), self.light_rook_)
                elif c == 'n':
                    self.board_img_.paste(self.light_knight_, self._algnot_to_pixel(algnot), self.light_knight_)
                elif c == 'b':
                    self.board_img_.paste(self.light_bishop_, self._algnot_to_pixel(algnot), self.light_bishop_)
                elif c == 'q':
                    self.board_img_.paste(self.light_queen_, self._algnot_to_pixel(algnot), self.light_queen_)
                elif c == 'k':
                    self.board_img_.paste(self.light_king_, self._algnot_to_pixel(algnot), self.light_king_)
                elif c == 'p':
                    self.board_img_.paste(self.light_pawn_, self._algnot_to_pixel(algnot), self.light_pawn_)
                elif c == 'R':
                    self.board_img_.paste(self.dark_rook_, self._algnot_to_pixel(algnot), self.dark_rook_)
                elif c == 'N':
                    self.board_img_.paste(self.dark_knight_, self._algnot_to_pixel(algnot), self.dark_knight_)
                elif c == 'B':
                    self.board_img_.paste(self.dark_bishop_, self._algnot_to_pixel(algnot), self.dark_bishop_)
                elif c == 'Q':
                    self.board_img_.paste(self.dark_queen_, self._algnot_to_pixel(algnot), self.dark_queen_)
                elif c == 'K':
                    self.board_img_.paste(self.dark_king_, self._algnot_to_pixel(algnot), self.dark_king_)
                elif c == 'P':
                    self.board_img_.paste(self.dark_pawn_, self._algnot_to_pixel(algnot), self.dark_pawn_)

    def _add_img(self, path):
        return Image.open(path)

    def _select(self, x, y):
        select_candidate = Square(int(x / 100) + 1 + (7 - int(y / 100)) * 8)  # equation for converting pixel to square
        piece_char = self.board_.piece(select_candidate)
        if (self.board_.side2move() == Color.White and piece_char.islower()) or (self.board_.side2move() == Color.Black and piece_char.isupper()):
            self.selected_ = select_candidate
        else:
            self.selected_ = Square.NoSquare

    def _redraw(self):
        self.tk_board_ = ImageTk.PhotoImage(self.board_img_)
        self.context_.board_label.configure(image=self.tk_board_)
        self.context_.board_label.image = self.tk_board_

    def _draw_selection(self):
        self.board_img_ = self._add_img("res/ChessBoard.png") # wipes board_img_ clean
        if self.selected_ is not Square.NoSquare:
            i, j = square_to_ind(self.selected_)
            algnot = chr(j + ord('a')) + chr(i + ord('1'))
            self.board_img_.paste(self.gray_circle_, self._algnot_to_pixel(algnot), self.gray_circle_)
        self._draw_board()
        self._redraw()

    def get(self):
        self.tk_board_ = ImageTk.PhotoImage(self.board_img_)
        return self.tk_board_

    def click_handler(self, event):
        self._select(event.x, event.y)
        self._draw_selection()