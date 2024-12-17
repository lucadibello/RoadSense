export {};

type BumpInput = number[];

declare global {
  interface Window {
    ENV: {
      API_URL: string;
    };
  }
  namespace L {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    function heatLayer(data: BumpInput[]): any;
  }
}
