#!/usr/bin/env python3
# coding: utf-8

import sys

sys.path.append(".")
sys.path.append("build_debug")
sys.path.append("build")

import libhex_uct_ext as hex_uct

print(hex_uct.helloWorld())
aa = hex_uct.Move()
board = hex_uct.Board(5)
coord = hex_uct.Coord(-1,3)
print(aa, board)
print(board.getNumberOfPlayers())
print(board.graph)
print(hex_uct.countNodes(board.graph), hex_uct.countEdges(board.graph))
print(coord)


