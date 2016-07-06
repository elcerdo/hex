#!/usr/bin/env python3
# coding: utf-8

import sys

sys.path.append(".")
sys.path.append("build_debug")
sys.path.append("build")

import libhex_uct_ext as hex_uct

print(hex_uct.helloWorld())
aa = hex_uct.Move()
coord = hex_uct.Coord(-1,3)
print(aa)
print(coord, coord.x, coord.y)

board = hex_uct.Board(5)
print(board)
print(board.getNumberOfPlayers())
print(board.graph)
print(hex_uct.countNodes(board.graph), hex_uct.countEdges(board.graph))

state = hex_uct.BoardState(board)
print(state)

class Status:
    def __init__(self):
        self.states = []
    def updateState(self, state):
        self.states.append(state)
    def updateMessage(self, message):
        print("status", message)

class Player:
    def __init__(self):
        self.state = None
    def update(self, state):
        print("player", state)
        self.state = state
    def getMove(self, board, player):
        assert( self.state is not None )
        moves = hex_uct.getAvailableMoves(self.state)
        coords = []
        for move in moves:
            coord = hex_uct.getCoord(board, move)
            coords.append((coord.x, coord.y))
        print("player", board, player, coords)
        assert( moves )
        return moves[0]

status = Status()
player = Player()
game = hex_uct.Game(status, player)
winner = game.run(board)
print(winner)
print(len(status.states))
print(status.states)
