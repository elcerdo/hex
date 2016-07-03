#!/usr/bin/env python3
# coding: utf-8

import sys

sys.path.append(".")
sys.path.append("build_debug")
sys.path.append("build")

import libhex_uct_ext as hex_uct
import flask

app = flask.Flask(__name__)

@app.route("/")
def root():
    return flask.render_template("root.html")

