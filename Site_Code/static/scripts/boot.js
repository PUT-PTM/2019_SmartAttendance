$(document).ready(function () {
    let main = $('.Main_Container');
    let popups = $('.Popups_Container');
    popups.hide();
    main.fadeIn(1000);

    db_get_student_info();
    db_get_presence();
});
