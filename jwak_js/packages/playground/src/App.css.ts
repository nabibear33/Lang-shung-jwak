import { style } from "@vanilla-extract/css";

export const container = style({
  padding: 16,
  display: "flex",
  flexDirection: "column",
  gap: 16,
});

export const title = style({
  margin: 0,
});

export const footer = style({
  display: "flex",
  gap: 16,
});

export const io = style({
  flex: 1,
});

export const label = style({
  fontWeight: "bold",
});

export const buttonGroup = style({
  display: "flex",
  gap: 8,
  marginTop: 4,
});

export const button = style({
  padding: "8px 12px",
  cursor: "pointer",
});
