$(document).ready(function () {
    $('#GitHub').on("click", function () {
        window.open('https://github.com/PUT-PTM/2019_SmartAttendance', '_blank');
    });
    $('#PP').on("click", function () {
        window.open('https://www.put.poznan.pl', '_blank');
    });
    $('#But_Data').on("click", function () {
        db_get_content();
    });
    $('#But_Add').on("click", function () {
        let index = parseInt($('#Input_SID').val());
        let fName = $('#Input_fName').val();
        let lName = $('#Input_lName').val();
        db_add_row(index, fName, lName);
    });
    $('#But_Del').on("click", function () {
        let index = parseInt($('#Input_SID').val());
        db_delete_row(index);
    });
});