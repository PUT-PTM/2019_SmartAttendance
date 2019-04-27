$(document).ready(function () {
    $('#GitHub').on('click', function () {
        window.open('https://github.com/PUT-PTM/2019_SmartAttendance', '_blank');
    });

    $('#PP').on('click', function () {
        window.open('https://www.put.poznan.pl', '_blank');
    });

    $('#But_Data').on('click', function () {
        db_get_student_info();
        db_get_presence();
    });


    // Popup events
    function add_popup_listener(elemName, elemTitle, elemFunction) {
        $(elemName).on('click', function () {
            let popups = $('.Popups_Container');
            let title = String($('#Inputs_Title').html());

            if (popups.is(':visible') && title === elemTitle) {
                popup_fadeout(reset_popup_position);
            } else {
                if (title !== elemTitle) {
                    popups.fadeOut(250, function () {
                        popup_fadein(elemTitle, elemFunction)
                    });
                } else {
                    popup_fadein(elemTitle, elemFunction);
                }
            }
        });
    }

    const popup_buttons = [
        ['#But_Add', 'Add student', popup_inputs_add],
        ['#But_Del', 'Delete student', popup_inputs_delete]
    ];
    popup_buttons.forEach(function (entry) {
        add_popup_listener(entry[0], entry[1], entry[2]);
    });

    function reset_popup_position() {
        let popups = $('.Popups_Container');
        popups.css('top', '200px');
        popups.css('left', 'calc(50%)');
    }

    $('#Cancel').on('click', function () {
        let popups = $('.Popups_Container');
        if (popups.is(':visible')) {
            popup_fadeout(reset_popup_position);
        }
    });

    $('.Content_Background').on('click', function () {
        if ($('.Popups_Container').is(':visible')) {
            $('#Cancel').click();
        }
    });

    $('.Tables').on('click', function () {
        if ($('.Popups_Container').is(':visible')) {
            $('#Cancel').click();
        }
    });


    let inputsPressed = false;
    let posPrev = null;

    $('.Inputs').on('mousedown', function (e) {
        inputsPressed = true;
        posPrev = [e.pageX, e.pageY];

        let popups = $('.Popups_Container');
        let content = $('.Content_Container');

        if (popups.is(':visible')) {
            $(document).on('mousemove', function (e) {
                if (popups.is(':visible')) {
                    const pos = [e.pageX, e.pageY];
                    const posElem = popups.position();
                    const posContent = content.position();
                    const contentBottom = posContent.top + content.height();

                    let newTopPos = posElem.top;

                    // Top and bottom safeguards
                    if (pos[1] > posContent.top) {
                        newTopPos += pos[1] - posPrev[1];
                    } else {
                        newTopPos = posContent.top;
                    }
                    if (newTopPos < posContent.top){
                        newTopPos = posContent.top;
                    }

                    if (pos[1] + popups.height() > contentBottom) {
                        newTopPos = contentBottom - popups.height();
                    }
                    popups.css('top', newTopPos);

                    let newLeftPos = pos[0] - popups.width() / 2;

                    // Left and right safeguards
                    if (newLeftPos < 0) {
                        newLeftPos = 0;
                    } else if (newLeftPos > content.width() - popups.width()) {
                        newLeftPos = content.width() - popups.width();
                    }
                    popups.css('left', newLeftPos);
                    posPrev = pos;
                }
            });
        }
    });
    $(document).on('mouseup', function () {
        if (inputsPressed) {
            $(document).off('mousemove');
            posPrev = null;
        }
    });
});

// Mouseover event atachment and handling
$(document).ready(function () {
    $('button').on('mouseenter', function () {
        $(this).css({'backgroundColor': 'lightsteelblue'});
    });
    $('#Cancel').on('mouseenter', function () {
        $(this).css('filter', 'brightness(80%)');
    });
});

// Mouseout event atachment and handling
$(document).ready(function () {
    $('button').on('mouseleave', function () {
        $(this).css({'backgroundColor': 'steelblue'});
    });
    $('#Cancel').on('mouseleave', function () {
        $(this).css('filter', 'brightness(100%)');
    });
});

$(window).ready(function () {
    $(this).keydown(function (e) {
            if ($('.Popups_Container').is(':visible')) {
                if (e.keyCode === 27) {
                    $('#Cancel').click();
                } else if (e.keyCode === 13) {
                    $('#But_Execute').click();
                }
            }
        }
    );
    $(this).keypress(function (e) {
        if (e.keyCode === 8) {
            e.preventDefault();
        }
    });
    $(this).keydown(function (e) {
        if (e.keyCode === 8) {
            e.preventDefault();
        }
    });
});
