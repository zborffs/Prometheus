Opening Book
============

Client Interface
--------------
- actual book data is deserialized from some file somewhere  
- client asks the book for all the moves from a given position
- client gets the moves, applies some function for selecting which of the moves to do, and tells the engine which of 
  the BookEdges he selected
- book looks at index of BookEdge in current BookNodes array corresponding to the move that the user has said he 
  sekected, then looks at the index associated with that move (which is stored in the BookEdge), and pushes that 
  BookNode onto a stack
- the player undoes the book move, the stack gets popped
    - the stack must be initialized with the starting position
- if the player starts from a position when the initial position is not the 'startpos', then it must do a search
    - until I think of a better strategy that'll just be a brute force search
    
Book Maker
----------
- make the first node the board.key() from the starting position and set both the games played and games won to zero
  and the book edges to null
- parse the protobook
    - convert the algebraic notation to a ChessMove
    - add the ChessMove object to the current node's list, setting the index to 0, i.e. the startpos
    - if the vector of parsed algebraic notations is over, then go to the next line, otherwise make the move and repeat
    