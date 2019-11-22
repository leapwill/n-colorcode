extern crate ncurses;
extern crate rand;

use ncurses::*;
use std::*;

pub const COLOR_GRAY: i16 = 8;

fn main() {
    let ans: [i16; 4] = [
        (rand::random::<u16>() % 6 + 1) as i16,
        (rand::random::<u16>() % 6 + 1) as i16,
        (rand::random::<u16>() % 6 + 1) as i16,
        (rand::random::<u16>() % 6 + 1) as i16,
    ];
    let colors: [&'static str; 7] = ["", "Red", "Green", "Yellow", "Blue", "Magenta", "Cyan"];

    // TODO options
    let num_attempts: u8 = 10;

    // window setup
    initscr();
    if has_colors() == false {
        endwin();
        println!("Your terminal does not support color.");
        process::exit(1);
    }

    start_color();
    init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_WHITE);
    init_color(COLOR_GRAY, 200, 200, 200);
    init_pair(COLOR_GRAY, COLOR_GRAY, COLOR_BLACK);

    cbreak();
    noecho();
    keypad(stdscr(), true);
    bkgd(COLOR_PAIR(COLOR_BLACK));

    // game setup
    let mut game: vec::Vec<vec::Vec<i16>> = vec::Vec::new();
    game.push(vec![
        COLOR_RED,
        COLOR_GREEN,
        COLOR_YELLOW,
        COLOR_BLUE,
        COLOR_MAGENTA,
        COLOR_CYAN,
    ]);
    /* [i16; 4]; num_attempts]; */
    for _ in 0..num_attempts as usize {
        game.push(vec![0i16; 4]);
    }

    let mut selector1: [i32; 2] = [4; 2];
    for i in 1..7 {
        print_color_block(selector1[0], selector1[1], i);
        selector1[0] += 3;
    }
    selector1[0] = 4;
    selector1[1] = 7;
    mvaddch(selector1[0], selector1[1], '*' as chtype);

    for i in 0..23 {
        mvaddch(i, 10, ACS_VLINE());
    }

    let mut selector2: [i32; 2] = [2, 14];
    for i in (selector2[0]..selector2[0] + 2 * num_attempts as i32).step_by(2) {
        for j in selector2[1]..selector2[1] + 16 {
            if (j - selector2[1]) % 4 != 0 {
                mvaddch(i, j, '_' as chtype);
            }
        }
    }

    selector2[0] = 3;
    selector2[1] = 16;
    mvaddch(selector2[0], selector2[1], '*' as chtype);
    wmove(stdscr(), 23, 79);
    let mut attempt: u8 = 1;
    // game loop
    loop {
        let inp: i32 = getch();
        // clear message
        wmove(stdscr(), 23, 0);
        clrtoeol();

        // handle input
        if inp == 'q' as i32 {
            break;
        }
        #[allow(unreachable_code)]
        {
            match inp {
                259 => {
                    // arrow up
                    mvaddch(selector1[0], selector1[1], ' ' as chtype);
                    selector1[0] -= 3;
                    if selector1[0] < 4 {
                        selector1[0] = 19;
                    }
                    mvaddch(selector1[0], selector1[1], '*' as chtype);
                },
                258 => {
                    // arrow down
                    mvaddch(selector1[0], selector1[1], ' ' as chtype);
                    selector1[0] += 3;
                    if selector1[0] > 19 {
                        selector1[0] = 4;
                    }
                    mvaddch(selector1[0], selector1[1], '*' as chtype);
                },
                32 => { // space (select)
                    // translate selector1 to color
                    let idx1 = (selector1[0] - 4) / 3;
                    // TODO debug mode
                    // printMsg("selected color idx %d", idx1);
                    let color = game[0 as usize][idx1 as usize];

                    // translate selector2 to board slot
                    let idx2 = (selector2[1] - 16) / 4;
                    game[attempt as usize][idx2 as usize] = color;

                    // put color on board
                    print_color_block(selector2[0] - 1, selector2[1] - 1, color);
                    // fallthrough to arrow right
                    move_right(&mut selector2);
                },
                261 => { // arrow right
                    move_right(&mut selector2);
                },
                260 => { // arrow left
                    mvaddch(selector2[0], selector2[1], ' ' as chtype);
                    selector2[1] -= 4;
                    if selector2[1] < 16 {
                        selector2[1] = 28;
                    }
                    mvaddch(selector2[0], selector2[1], '*' as chtype);
                },
                10 => { // enter (submit)
                    if game[attempt as usize][0] == 0 || game[attempt as usize][1] == 0 || game[attempt as usize][2] == 0 || game[attempt as usize][3] == 0 {
                        print_msg("incomplete attempt");
                        continue;
                    } 

                    let mut result: [u8; 4] = [0; 4];
                    let mut used_ans: [bool; 4] = [false; 4];
                    // calculate result
                    // first set used_ans based on exact match
                    for i in 0..4 {
                        let color = game[attempt as usize][i];
                        if color == ans[i] as i16 {
                            used_ans[i] = true;
                            result[i] = 2;
                        }
                    }
                    // then set wrong-location match
                    for i in 0..4 {
                        let color = game[attempt as usize][i];
                        for a in 0..4 {
                            if used_ans[a as usize] == false && color == ans[a as usize] && result[i] == 0 { // don't reuse ans[x] or overwrite result[x]
                                used_ans[a as usize] = true;
                                result[i] = 1;
                                continue;
                            }
                        }
                    }

                    // sort result for obfuscation
                    result.sort();
                    result.reverse();

                    // show result
                    wmove(stdscr(), selector2[0] - 1 as i32, 33);
                    for i in 0..4 {
                        match result[i] {
                            1 => { // color right but not location
                                addch(ACS_CKBOARD() | COLOR_PAIR(COLOR_GRAY));
                            },
                            2 => { // color and location right
                                addch(ACS_CKBOARD() | COLOR_PAIR(COLOR_WHITE));
                            },
                            _ => {}
                        }
                    }
                    // check win/lose
                    if result[0] == 2 && result[1] == 2 && result[2] == 2 && result[3] == 2 {
                        print_msg("win");
                        getch();
                        endwin();
                        return;
                    }
                    else if attempt >= num_attempts {
                        // lose, out of attempts
                        print_msg(&(format!("lose, answer was {} {} {} {}", colors[ans[0] as usize], colors[ans[1] as usize], colors[ans[2] as usize], colors[ans[3] as usize])));
                        getch();
                        endwin();
                        return;
                    }
                    else {
                        attempt += 1;
                        mvaddch(selector2[0], selector2[1], ' ' as chtype);
                        selector2[0] += 2;
                        mvaddch(selector2[0], selector2[1], '*' as chtype);
                    }
                },
                _ => {}
            }
            wmove(stdscr(), 23, 79);
            refresh();
        }
    }

    // cleanup
    endwin();
}

fn print_color_block(y: i32, x: i32, color_pair: i16) {
    wmove(stdscr(), y, x);
    addch(ACS_CKBOARD() | COLOR_PAIR(color_pair));
    addch(ACS_CKBOARD() | COLOR_PAIR(color_pair));
    addch(ACS_CKBOARD() | COLOR_PAIR(color_pair));
}

fn print_msg(msg: &str) {
    wmove(stdscr(), 23, 0);
    wprintw(stdscr(), msg);
}

fn move_right(selector2: &mut [i32; 2]) {
    mvaddch(selector2[0], selector2[1], ' ' as chtype);
    selector2[1] += 4;
    if selector2[1] > 28 {
        selector2[1] = 16;
    }
    mvaddch(selector2[0], selector2[1], '*' as chtype);
}
