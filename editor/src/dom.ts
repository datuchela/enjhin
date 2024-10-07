import { setCurrentBodyId } from "./state.js";
import type { EventWithValue, State } from "./types.js";

export function refreshListView(state: State) {
  const { element: { listView }, data: { bodies } } = state;
  listView.innerHTML = "";

  bodies.forEach((body) => {
    const listItem = document.createElement("li");
    const itemRadio = document.createElement("input");
    const itemLabel = document.createElement("label");

    listItem.classList.add("flex", "gap-2", "p-3");

    itemRadio.type = "radio";
    itemRadio.id = body.id;
    itemRadio.name = "body";
    itemRadio.value = body.id;
    itemRadio.onchange = (e) => {
      setCurrentBodyId((e as EventWithValue).target.value, state);
      console.log(state.currentBodyId);
    };
    itemRadio.checked = state.currentBodyId === body.id;

    itemLabel.htmlFor = body.id;
    itemLabel.innerText = `Body ${body.id}`;

    listItem.appendChild(itemRadio);
    listItem.appendChild(itemLabel);
    listView.appendChild(listItem);
  });
}

export function refreshButtons(state: State) {
  state.buttons.forEach((btn) => {
    btn.element.disabled = false;
    if (btn.mode !== state.mode) return;
    btn.element.disabled = true;
  });
}

