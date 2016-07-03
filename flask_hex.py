#!/usr/bin/env python3
# coding: utf-8

import sys

sys.path.append(".")
sys.path.append("build_debug")
sys.path.append("build")

import libhex_uct_ext as hex_uct
import flask

app = flask.Flask(__name__, static_folder="static")
app.secret_key = "0oqtjWXYAv0OIz3LjBGoap4xiA/m-5IvJ"

games = []

@app.route("/")
def root():
    return flask.render_template("root.html", route_name="root", games=games)

@app.route("/newgame", methods=["GET", "POST"])
def newgame():
    if flask.request.method == "POST":
        try:
            player_name = flask.request.form["name"] if flask.request.form["name"] else "default name"
            player_is_white = flask.request.form["color"] == "white"
            uct_time = float(flask.request.form["uct_time"])
            uct_prune = flask.request.form["uct_prune"] == "on"
            uct_constant = float(flask.request.form["uct_constant"])
            print(flask.request.form)
            print(player_name, player_is_white)
            print(uct_time, uct_prune, uct_constant)
            games.append((player_name, player_is_white, uct_time, uct_prune, uct_constant))
            flask.flash("new game created")
            return flask.redirect(flask.url_for('root'))
        except ValueError:
            flask.flash("invalid new game")
    return flask.render_template("newgame.html", route_name="newgame")

