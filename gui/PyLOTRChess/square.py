from enum import Enum


class Square(Enum):
    A1 = 1
    A2 = 9
    A3 = 17
    A4 = 25
    A5 = 33
    A6 = 41
    A7 = 49
    A8 = 57
    B1 = 2
    B2 = 10
    B3 = 18
    B4 = 26
    B5 = 34
    B6 = 42
    B7 = 50
    B8 = 58
    C1 = 3
    C2 = 11
    C3 = 19
    C4 = 27
    C5 = 35
    C6 = 43
    C7 = 51
    C8 = 59
    D1 = 4
    D2 = 12
    D3 = 20
    D4 = 28
    D5 = 36
    D6 = 44
    D7 = 52
    D8 = 60
    E1 = 5
    E2 = 13
    E3 = 21
    E4 = 29
    E5 = 37
    E6 = 45
    E7 = 53
    E8 = 61
    F1 = 6
    F2 = 14
    F3 = 22
    F4 = 30
    F5 = 38
    F6 = 46
    F7 = 54
    F8 = 62
    G1 = 7
    G2 = 15
    G3 = 23
    G4 = 31
    G5 = 39
    G6 = 47
    G7 = 55
    G8 = 63
    H1 = 8
    H2 = 16
    H3 = 24
    H4 = 32
    H5 = 40
    H6 = 48
    H7 = 56
    H8 = 64
    NoSquare = 65

    def __int__(self):
        return self.value


def square_to_ind(sq):
    sq_int = int(sq) - 1
    i = int(sq_int / 8)
    j = int(sq_int % 8)
    return i, j
