#pragma once
#define WM_SET_COMBO	WM_USER + 0x1FF1//a combobox lista kitolteset kero fuggveny uzenete
#define WM_SET_COLUMN_COLOR	WM_USER + 0x1FF2//a listcontrol oszlopainak szinet kero fuggveny uzenete
#define WM_SET_ROW_COLOR	WM_USER + 0x1FF3//a listcontrol oszlopainak szinet kero fuggveny uzenete
#define WM_ITEM_UPDATED	WM_USER + 0x1FF4//az editalast koveto valtozast jelento fv uzenete
#define WM_LISTCTRL_MENU WM_USER + 0x1FF5//a jobb gombbal kattintast jelento uzenet
#define WM_CLICKED_COLUMN WM_USER + 0x1FF6//rendezes utan a rendezett oszlop szamat adja vissza ez az uzenet (utoljara clickelt oszlop)
#define WM_DATA_CHANGED WM_USER + 0x1FF7//a listctrl adatainak megvaltozasarol kuld uzenetet, de csak sajat maganak
#define WM_ROWTODELETE WM_USER + 0x1FF8//a listctrl delete gombbal torteno esemenyerol kuld infot
//#define WM_FILL_TABLE WM_USER + 0x1FF9//a Table_firstname-nek küld üzenetet, hogy újra kell listázni az adatbázist
//#define WM_MAIN_TITLE WM_USER + 0x1FF9//a listctrl delete gombbal torteno esemenyerol kuld infot
//#define WM_SYNCRONIZE_PEOPLE WM_USER + 0x1FFA 
