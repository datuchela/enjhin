import {
  type State,
  type Spring,
  type Vector2,
  type Particle,
  type SoftBody,
  type SpringId,
  type ParticleId,
  type SoftBodyId,
  type Color,
  Mode,
} from "./types.js";

const CANVAS_WIDTH = 1366;
const CANVAS_HEIGHT = 800;
const CANVAS_BACKGROUND = "black";
const PARTICLE_COLOR = "#FCE93D";
const PARTICLE_COLOR_FOCUS = "#D9CB40";

(() => {
  const canvas = document.getElementById("enjhin-editor") as HTMLCanvasElement | null;
  const selectBtn = document.getElementById("mode-select") as HTMLButtonElement | null;
  const insertBtn = document.getElementById("mode-insert") as HTMLButtonElement | null;
  const moveBtn = document.getElementById("mode-move") as HTMLButtonElement | null;
  const exportBtn = document.getElementById("export") as HTMLButtonElement | null;
  const newBodyBtn = document.getElementById("new-body") as HTMLButtonElement | null;

  if (!canvas) throw new Error("Couldn't get canvas");
  if (!selectBtn) throw new Error("Couldn't get selectBtn");
  if (!insertBtn) throw new Error("Couldn't get insertBtn");
  if (!moveBtn) throw new Error("Couldn't get moveBtn");
  if (!exportBtn) throw new Error("Couldn't get exportBtn");
  if (!newBodyBtn) throw new Error("Couldn't get newBodyBtn");

  canvas.width = CANVAS_WIDTH;
  canvas.height = CANVAS_HEIGHT;

  const ctx = canvas.getContext("2d");
  if (!ctx) throw new Error(`Couldn't get 2d context from canvas: ${canvas}`);

  const state: State = {
    mode: Mode.SELECT,
    data: {
      bodies: [],
      springs: [],
      particles: [],
    },
    currentBodyId: null,
    currentSpringId: null,
    currentParticleId: null,
    element: {
      canvas: canvas,
      ctx: ctx,
    },
    buttons: [
      { mode: Mode.SELECT, element: selectBtn },
      { mode: Mode.INSERT, element: insertBtn },
      { mode: Mode.MOVE, element: moveBtn },
    ],
    idDispatcher: {
      bodyIdCounter: 0,
      springIdCounter: 0,
      particleIdCounter: 0,
      getNewId: (idCounter: number) => {
        return ++idCounter;
      },
    }
  };

  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

  canvas.addEventListener("click", (e) => handleMouseClick(e, state));
  exportBtn.addEventListener("click", (e) => handleExportClick(e, state));
  newBodyBtn.addEventListener("click", () => handleNewBodyClick(state));
  state.buttons.forEach((btn) => {
    btn.element.addEventListener("click", () => handleModeClick(btn.mode, state));
  });

  redrawAll(state);
})();

function handleNewBodyClick(state: State) {
  const newBody: SoftBody = {
    id: `${state.idDispatcher.getNewId(state.idDispatcher.bodyIdCounter)}`,
    springs: [],
    particles: [],
  };
  addBody(newBody, state);
  setCurrentBodyId(newBody.id, state);
}

function refreshButtons(state: State) {
  state.buttons.forEach((btn) => {
    btn.element.disabled = false;
    if (btn.mode !== state.mode) return;
    btn.element.disabled = true;
  });
}

function handleModeClick(mode: Mode, state: State) {
  setMode(mode, state);
  refreshButtons(state);
}

function handleExportClick(e: MouseEvent, state: State) {
  e.preventDefault();
  console.log(state.data);
}

function handleMouseClick(e: MouseEvent, state: State) {
  const rect = state.element.canvas.getBoundingClientRect();
  const position: Vector2 = { x: e.clientX - rect.left, y: e.clientY - rect.top };
  if (position.x < 0) {
    position.x = 0;
  }
  if (position.y < 0) {
    position.y = 0;
  }

  switch (state.mode) {
    case Mode.MOVE:
      console.log("MOVE mode");
      break;
    case Mode.SELECT:
      console.log("SELECT mode");
      break;
    case Mode.INSERT:
      if (state.currentBodyId === null) break;
      console.log("INSERT mode");
      addParticle(
        {
          id: `${state.idDispatcher.getNewId(state.idDispatcher.particleIdCounter)}`,
          position,
        },
        state.currentBodyId,
        state
      );
      break;
  }

  redrawAll(state);

}

function getBodyById(id: SoftBodyId, state: State) {
  const { bodies } = state.data;

  const body = bodies.find((body) => body.id === id);
  if (!body) throw new Error(`Couldn't find body with id ${id}`);
  return body;
}

function getParticleById(id: ParticleId, state: State) {
  const { particles } = state.data

  const particle = particles.find((particle) => particle.id === id);
  if (!particle) throw new Error(`Couldn't find particle with id ${id}`);
  return particle;
}

function getSpringById(id: SpringId, state: State) {
  const { springs } = state.data

  const spring = springs.find((spring) => spring.id === id);
  if (!spring) throw new Error(`Couldn't find spring with id ${id}`);
  return spring;
}

function addBody(body: SoftBody, state: State) {
  state.data.bodies.push(body);
}

function addSpringToBody(spring: Spring, bodyId: SoftBodyId, state: State) {
  const body = getBodyById(bodyId, state);
  body.springs.push(spring.id);
}

function addParticle(particle: Particle, bodyId: SoftBodyId, state: State) {
  const body = getBodyById(bodyId, state);
  body.particles.push(particle.id);
}

function setCurrentBodyId(id: SoftBodyId, state: State) {
  state.currentBodyId = id;
}

function setCurrentParticleId(id: ParticleId, state: State) {
  state.currentParticleId = id;
}

function setCurrentSpringId(id: SpringId, state: State) {
  state.currentSpringId = id;
}

function setMode(mode: Mode, state: State) {
  state.mode = mode;
}

function redrawAll(state: State) {
  redrawCanvas(state);
  refreshButtons(state);
}

function redrawCanvas(state: State) {
  const { element: elements, data: { bodies } } = state;
  const { ctx } = elements;

  clearBackground(ctx, CANVAS_BACKGROUND);

  for (let i = 0; i < bodies.length; ++i) {
    for (let j = 0; j < bodies[i].springs.length; ++j) {
      const spring = getSpringById(bodies[i].springs[j], state);
      const particle1 = getParticleById(spring.particles[0], state);
      const particle2 = getParticleById(spring.particles[1], state);
      drawLine(ctx, particle1.position, particle2.position, "white");
    }
    for (let k = 0; k < bodies[i].particles.length; ++k) {
      const particle = getParticleById(bodies[i].particles[k], state);
      let color = (particle.id === state.currentParticleId) ? PARTICLE_COLOR_FOCUS : PARTICLE_COLOR;
      drawCircle(ctx, particle.position, 5, color);
    }
  }
}

function drawCircle(ctx: CanvasRenderingContext2D, center: Vector2, radius: number, color: Color) {
  ctx.fillStyle = color;
  ctx.beginPath();
  ctx.arc(center.x, center.y, radius, 0, 2 * Math.PI);
  ctx.fill();
}

function drawLine(ctx: CanvasRenderingContext2D, start: Vector2, end: Vector2, color: Color, lineWidth?: number) {
  ctx.strokeStyle = color;
  ctx.lineWidth = lineWidth ?? 1;
  ctx.moveTo(start.x, start.y);
  ctx.lineTo(end.x, end.y);
  ctx.stroke();
}

function clearBackground(ctx: CanvasRenderingContext2D, color: Color) {
  ctx.fillStyle = color;
  ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
}
