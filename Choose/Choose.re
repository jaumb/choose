open Revery;
open Revery.UI;
open Revery.UI.Components;

type todo = {
  id: int,
  task: string,
  isDone: bool,
};

module Filter = {
  type t =
    | All
    | Completed
    | NotCompleted;

  let show = (v: t) =>
    switch (v) {
    | All => "All"
    | Completed => "Completed"
    | NotCompleted => "NotCompleted"
    };
};

type state = {
  todos: list(todo),
  filter: Filter.t,
  inputValue: string,
  nextId: int,
};

type action =
  | AddTodo
  | ChangeFilter(Filter.t)
  | UpdateInputTextValue(string)
  | ChangeTaskState(int, bool);

let reducer = (action: action, state: state) =>
  switch (action) {
  | AddTodo => {
      ...state,
      todos: [
        {id: state.nextId, task: state.inputValue, isDone: false},
        ...state.todos,
      ],
      inputValue: "",
      nextId: state.nextId + 1,
    }
  | UpdateInputTextValue(text) => {...state, inputValue: text}
  | ChangeTaskState(id, isDone) =>
    let todos =
      List.map(
        item => item.id == id ? {...item, isDone} : item,
        state.todos,
      );
    {...state, todos};
  | ChangeFilter(filter) => {...state, filter}
  };

module Choose = {
  let component = React.component("Choose");

  let createElement = (~children as _, ()) =>
    component(hooks => {
      let ({todos, inputValue, filter, _}, dispatch, hooks) =
        React.Hooks.reducer(
          ~initialState={todos: [], filter: All, inputValue: "", nextId: 0},
          reducer,
          hooks,
        );

      let hooks =
        React.Hooks.effect(
          OnMount,
          () => {
            let unsubscribe = () => ();
            Some(unsubscribe);
          },
          hooks,
        );

      let renderTodo = task =>
        <View style=Style.[flexDirection(`Row)]>
          <Checkbox
            checked={task.isDone}
            onChange={checked => dispatch(ChangeTaskState(task.id, checked))}
          />
          <Text
            style=Style.[color(Colors.black), fontSize(20), margin(4)]
            text={task.task}
          />
        </View>;

      let filteredList =
        List.filter(
          task =>
            switch (filter) {
            | All => true
            | Completed => task.isDone
            | NotCompleted => !task.isDone
            },
          todos,
        );

      let listOfTodos = List.map(renderTodo, filteredList);
      (
        hooks,
        <View
          style=Style.[
            position(`Absolute),
            top(0),
            bottom(0),
            left(0),
            right(0),
            alignItems(`Center),
            justifyContent(`Center),
            flexDirection(`Column),
            backgroundColor(Colors.white),
          ]>
          <View style=Style.[flexDirection(`Row)]>
            <Input
              style=Style.[width(400)]
              placeholder="Add your Todo here"
              value=inputValue
              onChange={
                ({value, _}) => dispatch(UpdateInputTextValue(value))
              }
            />
            <Button
              width=50
              height=50
              disabled={
                switch (inputValue) {
                | "" => true
                | _ => false
                }
              }
              title="+"
              onClick={() => dispatch(AddTodo)}
            />
          </View>
          <ScrollView
            style=Style.[
              height(200),
              width(450),
              border(~width=1, ~color=Colors.black),
            ]>
            <View> ...listOfTodos </View>
          </ScrollView>
        </View>,
      );
    });
};

let init = app => {
  let win =
    App.createWindow(
      ~createOptions=
        WindowCreateOptions.create(
          ~resizable=false,
          ~visible=true,
          ~maximized=false,
          ~decorated=false,
          ~width=800,
          ~height=600,
          ~backgroundColor=Colors.white,
          ~vsync=false,
          ~icon=None,
          (),
        ),
      app,
      "Choose",
    );

  let _ = UI.start(win, <Choose />);
  ();
};

App.start(init);
