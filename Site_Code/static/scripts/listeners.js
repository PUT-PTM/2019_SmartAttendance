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
    $('#But_Add').on("click", function () {
        /*let index = parseInt($('#Input_SID').val());
        let fName = $('#Input_fName').val();
        let lName = $('#Input_lName').val();
        db_add_row(index, fName, lName);*/
        let popups = $('.Popups_Container');
        if (popups.is(':visible')) {
            popups_fadeout();
        } else {
            popups_fadein('Add student', popups_inputs_add);
        }
    });
    $('#Cancel').on("click", function () {
        let popups = $('.Popups_Container');
        if (popups.is(':visible')) {
            popups_fadeout();
        }
    });
    $('#But_Del').on("click", function () {
        popups_fadein('Delete student', popups_inputs_delete);
        //let index = parseInt($('#Input_SID').val());
        //db_delete_row(index);
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
});

//Mouseout event atachment and handling
$(document).ready(function () {
    $("button").on("mouseleave", function () {
        $(this).css({'backgroundColor': "steelblue"});
    });
});