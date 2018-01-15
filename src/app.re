[%bs.raw {|require('./app.css')|}];

type state = { quote: string, author: string };

type action = NextQuote(state) | Click | NoOp;

let parseResponse = (json: Js.Json.t) : state =>
  Json.Decode.{
    quote: field("quote", string, json),
    author: field("author", string, json)
  };

let component = ReasonReact.reducerComponent("App");

let make = _children => {

  let fetchQuote = (self) => {
    Js.Promise.(
      Fetch.fetch("https://randomquoteapi.glitch.me/funny")
      |> then_(Fetch.Response.text)
      |> then_(text => {
          let result = parseResponse(Js.Json.parseExn(text));
          self.ReasonReact.send(NextQuote(result));
          Js.Promise.resolve();
        })
    ) |> ignore;
  };

  {
  ...component,
  initialState: () => {
    quote: "Hello, world!",
    author: "Anonymous",
  },
  reducer: (action, _state) => {
    switch action {
    | NextQuote(result) => ReasonReact.Update(result);
    | Click => ReasonReact.SideEffects(fetchQuote);
    | NoOp => ReasonReact.NoUpdate;
    }
  },
  didMount: (self) => {
    fetchQuote(self);
    ReasonReact.NoUpdate;
  },
  render: self =>
    <div className="App">
      <div>(ReasonReact.stringToElement(self.state.quote)) </div>
      <div>{ReasonReact.stringToElement(self.state.author)}</div>
      <button onClick=(_event => self.send(Click))>{ReasonReact.stringToElement("Fetch New Quote")}</button>
    </div>
  }
};
