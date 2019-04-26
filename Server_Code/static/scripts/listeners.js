$(document).ready(function () {
    $('#GitHub').on("click", function () {
        window.open('https://github.com/PUT-PTM/2019_SmartAttendance', '_blank');
    });

    $('#PP').on("click", function () {
        window.open('https://www.put.poznan.pl', '_blank');
    });

    $('#But_Data').on("click", function () {
        db_get_student_info();
        db_get_presence();
    });

    
    function add_popup_listener(elemName, elemTitle, elemFunction)
    {
        $(elemName).on("click", function () {
            let popups = $('.Popups_Container');
            let title = String($('#Inputs_Title').html());

            if (popups.is(':visible') && title === elemTitle) {
                popups_fadeout();
            } else {
                if (title !== elemTitle) {
                    popups.fadeOut(250, function () {
                        popups_fadein('Add student', elemFunction)
                    });
                } else {
                    popups_fadein('Add student', elemFunction);
                }
            }
        });
    }

    const popup_buttons = [
          ['#But_Add', 'Add stuudent', popups_inputs_add],
          ['#But_Del', 'Delete stuudent', popups_inputs_delete]
      ];

    popup_buttons.forEach(function (entry) {
        add_popup_listener(entry[0], entry[1], entry[2]);
    });


    $('#Cancel').on("click", function () {
        let popups = $('.Popups_Container');
        if (popups.is(':visible')) {
            popups_fadeout();
        }
    });

    $('#But_Pres_Add').on("click", function () {
        let index = parseInt($('#Input_SID').val());
        let CID = parseInt($('#Input_CID').val());
        let room = parseInt($('#Input_Room').val());
        db_add_presence(index, CID, room);
    });
});

//Mouseover event atachment and handling
$(document).ready(function () {
    $("button").on("mouseenter", function () {
        $(this).css({'backgroundColor': "lightsteelblue"});
    });
    $("#Cancel").on("mouseenter", function () {
        $(this).css('filter', 'brightness(80%)');
    });
});

//Mouseout event atachment and handling
$(document).ready(function () {
    $("button").on("mouseleave", function () {
        $(this).css({'backgroundColor': "steelblue"});
    });
    $("#Cancel").on("mouseleave", function () {
        $(this).css('filter', 'brightness(100%)');
    });
});
