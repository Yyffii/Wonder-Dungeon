
// state controllers
static int baseState;
static int hoverState;

enum baseStates
{
    splash,
    title,
    game,
    over
};

enum hoverStates
{
    help,
    none,
    map,
    sign

};

// switches state
bool splashTimer(void)
{
    baseState = title;

    return false;
}

// clears hoverstate
bool clearHover(void)
{
    if (IsKeyReleased(KEY_ENTER) || IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
    {
        hoverState = none;
    }

    return false;
}
