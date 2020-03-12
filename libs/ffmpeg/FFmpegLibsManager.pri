win32 {
    CONFIG(debug, debug|release){
        FFMPEG_TARGET_PATH = $${OUT_PWD}/debug
    }else {
        FFMPEG_TARGET_PATH = $${OUT_PWD}/release
    }

    contains(QT_ARCH, x86_64){#64 bits
        INCLUDEPATH += $${PWD}/ffmpeg_4.0.2_win64/include
        LIBS += -L$${PWD}/ffmpeg_4.0.2_win64/lib \
                -lavcodec   \
                -lavdevice  \
                -lavfilter  \
                -lavformat  \
                -lavutil    \
                -lpostproc  \
                -lswresample\
                -lswscale

        QMAKE_POST_LINK += $$escape_expand(\\n) $${QMAKE_COPY_DIR} \"$${PWD}\\ffmpeg_4.0.2_win64\\bin\" \"$${FFMPEG_TARGET_PATH}\" $$escape_expand(\\n)
    }else {#32 bits
        INCLUDEPATH += $${PWD}/ffmpeg_4.0.2_win32/include
        LIBS += -L$${PWD}/ffmpeg_4.0.2_win32/lib \
                -lavcodec   \
                -lavdevice  \
                -lavfilter  \
                -lavformat  \
                -lavutil    \
                -lpostproc  \
                -lswresample\
                -lswscale

        QMAKE_POST_LINK += $$escape_expand(\\n) $${QMAKE_COPY_DIR} \"$${PWD}\\ffmpeg_4.0.2_win32\\bin\" \"$${FFMPEG_TARGET_PATH}\" $$escape_expand(\\n)
    }
}
