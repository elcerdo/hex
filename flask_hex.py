#!/usr/bin/env python3
# coding: utf-8

import sys

sys.path.append(".")
sys.path.append("build_debug")
sys.path.append("build")

import libhex_uct_ext as hex_uct
import flask

app = flask.Flask(__name__, static_folder="static")

games = []

@app.route("/")
def root():
    return flask.render_template("root.html", route_name="root")

@app.route("/newgame")
def newgame():
    return flask.render_template("newgame.html", route_name="newgame")

