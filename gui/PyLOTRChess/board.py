from .color import Color
from .square import Square, square_to_ind


class Board:
    def __init__(self, fen):
        self.board_ = [[]]
        self.side2move_ = Color.NoColor
        if fen == 'startpos':
            self._parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
        else:
            self._parse_fen(fen)

    def _parse_pos(self, pos):
        rows = pos.split('/')
        assert(len(rows) == 8)
        self.board_.clear()
        for i in range(0, 8):
            row_list = []
            row = rows[i]
            for j in range(0, len(row)):
                if row[j].isnumeric():
                    row_list.extend([' '] * int(row[j]))
                else:
                    row_list.append(row[j])
            self.board_.append(row_list)

    def _parse_color(self, color):
        if color == 'w':
            self.side2move_ = Color.White
        else:
            self.side2move_ = Color.Black

    def _parse_fen(self, fen):
        commands = fen.split(' ')
        self._parse_pos(commands[0])
        self._parse_color(commands[1])

    def board(self):
        return self.board_

    def __repr__(self):
        return str(self.board_)

    def __str__(self):
        return str(self.board_)

    def piece(self, sq):
        if sq is Square.NoSquare:
            return None
        i, j = square_to_ind(sq)
        return self.board_[i][j]

    def side2move(self):
        return self.side2move_