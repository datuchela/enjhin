export enum Mode {
  SELECT,
  INSERT,
  MOVE,
};

export type Color = string | CanvasGradient | CanvasPattern;

export type EventWithValue = Event & { target: { value: string } };

export type Vector2 = {
  x: number;
  y: number;
}

export type ParticleId = string;
export type SpringId = string;
export type SoftBodyId = string;

export type Particle = {
  id: ParticleId;
  position: Vector2
}

export type Spring = {
  id: SpringId;
  particles: [ParticleId, ParticleId];
}

export type SoftBody = {
  id: SoftBodyId;
  particles: ParticleId[];
  springs: SpringId[]
}

export type EngineData = {
  bodies: SoftBody[];
  particles: Particle[];
  springs: Spring[];
}

export type State = {
  mode: Mode;
  data: EngineData;
  currentBodyId: SoftBodyId | null;
  currentParticleId: ParticleId | null;
  currentSpringId: SpringId | null;
  element: {
    canvas: HTMLCanvasElement;
    ctx: CanvasRenderingContext2D;
  }
  buttons: { mode: Mode, element: HTMLButtonElement }[];
  idDispatcher: {
    bodyIdCounter: number,
    springIdCounter: number,
    particleIdCounter: number,
    getNewId: (idCounter: number) => number;
  }
}

