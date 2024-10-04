type EventWithValue = Event & { target: { value: string } };

type Vector2 = {
  x: number;
  y: number;
}

type Particle = {
  id: string;
  position: Vector2
}

type Spring = {
  id: string;
  particles: [Particle, Particle];
}

type SoftBody = {
  id: string;
  particles: Particle[];
  springs: Spring[]
}

enum Mode {
  SELECT,
  INSERT,
  DRAG,
};

type State = {
  mode: Mode;
  bodies: SoftBody[];
  currentBodyId: string | null;
  currentParticleIds: [string | null, string | null];
  elements: {
    canvas: HTMLCanvasElement;
    ctx: CanvasRenderingContext2D;
    bodyList: HTMLUListElement;
    bodySelector: HTMLSelectElement;
    particleSelectors: [HTMLSelectElement, HTMLSelectElement];
  }
}

const CANVAS_WIDTH = 1366;
const CANVAS_HEIGHT = 800;

let bodyIdCounter = 0;
let particleIdCounter = 0;
let springIdCounter = 0;


(() => {
  const canvas = document.getElementById("enjhin-editor") as HTMLCanvasElement | null;
  const exportBtn = document.getElementById("export-btn") as HTMLButtonElement | null;
  const newBodyBtn = document.getElementById("new-body-btn") as HTMLButtonElement | null;
  const bodyList = document.getElementById("body-list") as HTMLUListElement | null;
  const bodySelector = document.getElementById("body-select") as HTMLSelectElement | null;
  const particleSelector1 = document.getElementById("particle-select1") as HTMLSelectElement | null;
  const particleSelector2 = document.getElementById("particle-select2") as HTMLSelectElement | null;
  const attachBtn = document.getElementById("attach-btn") as HTMLButtonElement | null;

  if (!canvas) throw new Error("Couldn't get canvas");
  if (!exportBtn) throw new Error("Couldn't get toggleBtn");
  if (!newBodyBtn) throw new Error("Couldn't get toggleBtn");
  if (!bodyList) throw new Error("Couldn't get exportBtn");
  if (!bodySelector) throw new Error("Couldn't get exportBtn");
  if (!particleSelector1) throw new Error("Couldn't get particleSelector1");
  if (!particleSelector2) throw new Error("Couldn't get particleSelector2");
  if (!attachBtn) throw new Error("Couldn't get attachBtn");

  const ctx = canvas.getContext("2d");
  if (!ctx) throw new Error(`Couldn't get 2d context from canvas: ${canvas}`);

  const state: State = {
    mode: Mode.INSERT,
    bodies: [],
    currentBodyId: null,
    currentParticleIds: [null, null],
    elements: {
      canvas: canvas,
      ctx: ctx,
      bodyList: bodyList,
      bodySelector: bodySelector,
      particleSelectors: [particleSelector1, particleSelector2],
    }
  };

  canvas.width = CANVAS_WIDTH;
  canvas.height = CANVAS_HEIGHT;

  let oldCtxFillStyle = ctx.fillStyle;
  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
  ctx.fillStyle = oldCtxFillStyle;

  canvas.addEventListener("click", (e) => handleMouseClick(e, state));
  exportBtn.addEventListener("click", (e) => handleExportClick(e, state));
  newBodyBtn.addEventListener("click", (e) => handleNewBodyClick(e, state));
  bodySelector.addEventListener("change", (e) => handleBodySelectorChange(e as Event & { target: { value: string } }, state));
  particleSelector1.addEventListener("change", (e) => handleParticleSelectorChange(e as EventWithValue, state, 0));
  particleSelector2.addEventListener("change", (e) => handleParticleSelectorChange(e as EventWithValue, state, 1));
  attachBtn.addEventListener("click", (e) => handleAttachClick(e, state));
})();

function getBodyById(id: string, state: State) {
  const { bodies } = state;

  const body = bodies.find((body) => body.id === id);
  if (!body) throw new Error(`Couldn't find body with id ${id}`);
  return body;
}

function getParticleById(id: string, state: State) {
  for (let i = 0; i < state.bodies.length; ++i) {
    const particle = state.bodies[i].particles.find((p) => p.id === id);
    if (particle) return particle
  }
  return null
}

function addSpringToBody(spring: Spring, bodyId: string, state: State) {
  const body = getBodyById(bodyId, state);
  body.springs.push(spring);
}

function setCurrentBodyId(id: string, state: State) {
  state.currentBodyId = id;
  state.elements.bodySelector.value = id;
  refreshParticleSelectors(state);
}

function setCurrentParticleId(id: string, index: number, state: State) {
  state.currentParticleIds[index] = id;
  state.elements.particleSelectors[index].value = id;
}

function addBody(body: SoftBody, state: State) {
  state.bodies.push(body);

  const optionBody = document.createElement("option");
  optionBody.id = body.id;
  optionBody.value = body.id;
  optionBody.innerText = `Body #${body.id}`
  state.elements.bodySelector.appendChild(optionBody);
}

function addParticle(particle: Particle, bodyId: string, state: State) {
  const { bodies } = state;

  const body = bodies.find((body) => body.id === bodyId);
  if (!body) throw new Error(`Couldn't find body with id ${bodyId}`);

  body.particles.push(particle);
}

function refreshParticleSelectors(state: State) {
  const { currentBodyId, elements, currentParticleIds } = state;
  if (currentBodyId === null) return;

  const body = getBodyById(currentBodyId, state);

  for (let k = 0; k < 2; ++k) {
    elements.particleSelectors[k].innerHTML = "";
    for (let i = 0; i < body.particles.length; ++i) {
      const optionParticle = document.createElement("option");
      optionParticle.id = body.particles[i].id;
      optionParticle.value = body.particles[i].id;
      optionParticle.innerText = `Particle #${body.particles[i].id}`
      elements.particleSelectors[k].appendChild(optionParticle);
    }
  }
  currentParticleIds.forEach((id, i) => {
    if (!id) return;
    setCurrentParticleId(id, i, state);
  });
}

function redraw(state: State) {
  const { elements, bodies } = state;
  const { ctx } = elements;
  let savedCtxFillStyle = ctx.fillStyle;

  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

  for (let i = 0; i < bodies.length; ++i) {
    for (let k = 0; k < bodies[i].springs.length; ++k) {
      const spring = bodies[i].springs[k];
      ctx.strokeStyle = "white";
      ctx.moveTo(spring.particles[0].position.x, spring.particles[0].position.y);
      ctx.lineTo(spring.particles[1].position.x, spring.particles[1].position.y);
      ctx.stroke();
    }
    for (let j = 0; j < bodies[i].particles.length; ++j) {
      const particle = bodies[i].particles[j]
      if (particle.id === state.currentParticleIds[0]) {
        ctx.fillStyle = "red";
      } else if (particle.id === state.currentParticleIds[1]) {
        ctx.fillStyle = "blue"
      } else {
        ctx.fillStyle = "yellow";
      }
      ctx.beginPath();
      ctx.arc(particle.position.x, particle.position.y, 5, 0, 2 * Math.PI);
      ctx.fill();
    }
  }

  elements.ctx.fillStyle = savedCtxFillStyle;
}

function handleBodySelectorChange(e: EventWithValue, state: State) {
  setCurrentBodyId(e.target.value, state);
}

function handleParticleSelectorChange(e: EventWithValue, state: State, index: number) {
  setCurrentParticleId(e.target.value, index, state);
  redraw(state);
}

function handleAttachClick(e: MouseEvent, state: State) {
  e.preventDefault();
  const { currentParticleIds, currentBodyId } = state;
  if (
    currentParticleIds[0] === null ||
    currentParticleIds[1] === null ||
    currentParticleIds[0] === currentParticleIds[1] ||
    currentBodyId === null
  ) return;

  const particles: [Particle | null, Particle | null] = [
    getParticleById(currentParticleIds[0], state),
    getParticleById(currentParticleIds[1], state),
  ];
  if (!particles[0] && !particles[1]) return;

  const newSpring: Spring = { id: `${++springIdCounter}`, particles: particles as [Particle, Particle] }
  addSpringToBody(newSpring, currentBodyId, state);
  redraw(state);
}

function handleNewBodyClick(e: MouseEvent, state: State) {
  e.preventDefault();
  const newBody: SoftBody = { id: `${++bodyIdCounter}`, particles: [], springs: [] };
  addBody(newBody, state);
  setCurrentBodyId(newBody.id, state);
}

function handleExportClick(e: MouseEvent, state: State) {
  e.preventDefault();
  console.log({ bodies: state.bodies });
}

function handleMouseClick(e: MouseEvent, state: State) {
  const { currentBodyId } = state;
  if (currentBodyId === null) return;

  const rect = state.elements.canvas.getBoundingClientRect();
  const newParticle: Particle = {
    id: `${++particleIdCounter}`, position: { x: (e.clientX - rect.left), y: e.clientY - rect.top }
  };
  if (newParticle.position.x < 0) {
    newParticle.position.x = 0;
  }
  if (newParticle.position.y < 0) {
    newParticle.position.y = 0;
  }

  addParticle(newParticle, currentBodyId, state);

  for (let i = 0; i < state.currentParticleIds.length; ++i) {
    if (!state.currentParticleIds[i]) {
      state.currentParticleIds[i] = newParticle.id;
      console.log("before break", i);
      break;
    }
    console.log("after break inside for loop", i);
  }
  console.log("outside for loop");
  state.currentParticleIds.forEach((id, i) => {
    if (!id) state.currentParticleIds[i] = newParticle.id
  })

  redraw(state);
  refreshParticleSelectors(state);
}

