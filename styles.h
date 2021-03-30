#ifndef STYLES_H
#define STYLES_H

constexpr char buttonStyle[] = "QPushButton { "
              "background-color: rgb(235, 165, 0); "
              "border-width: none; "
              "} "
              "QPushButton:hover { "
              "background-color: rgb(255, 115, 0); "
              "} "
              "QPushButton:pressed { "
              "background-color: rgb(255, 185, 0); "
              "}";

constexpr char buttonToolStyle[] = "QToolButton { "
              "background-color: rgb(235, 165, 0); "
              "border-width: none; "
              "} "
              "QToolButton:hover { "
              "background-color: rgb(255, 115, 0); "
              "} "
              "QToolButton:pressed { "
              "background-color: rgb(255, 185, 0); "
              "}";
#endif // STYLES_H
