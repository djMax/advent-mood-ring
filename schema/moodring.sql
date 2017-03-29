--
-- PostgreSQL database dump
--

-- Dumped from database version 9.5.4
-- Dumped by pg_dump version 9.6.2

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner:
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner:
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: instruction; Type: TABLE; Schema: public; Owner: gbowner
--

CREATE TABLE instruction (
    user_id integer,
    instruction jsonb
);


ALTER TABLE instruction OWNER TO gbowner;

--
-- Name: user; Type: TABLE; Schema: public; Owner: gbowner
--

CREATE TABLE user (
    user_id integer NOT NULL,
    identifier text,
    "pixel_id" integer
);


ALTER TABLE user OWNER TO gbowner;

--
-- Name: user_id_seq; Type: SEQUENCE; Schema: public; Owner: gbowner
--

CREATE SEQUENCE user_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE user_id_seq OWNER TO gbowner;

--
-- Name: user_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gbowner
--

ALTER SEQUENCE user_id_seq OWNED BY user.user_id;


--
-- Name: user user_id; Type: DEFAULT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY user ALTER COLUMN user_id SET DEFAULT nextval('user_id_seq'::regclass);


--
-- Data for Name: instruction; Type: TABLE DATA; Schema: public; Owner: gbowner
--

COPY instruction (user_id, instruction) FROM stdin;
\.


--
-- Data for Name: user; Type: TABLE DATA; Schema: public; Owner: gbowner
--

COPY user (user_id, email, "pixelId") FROM stdin;
\.


--
-- Name: user_id_seq; Type: SEQUENCE SET; Schema: public; Owner: gbowner
--

SELECT pg_catalog.setval('user_id_seq', 1, false);


--
-- Name: instruction instruction_user_id_key; Type: CONSTRAINT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY instruction
    ADD CONSTRAINT instruction_user_id_key UNIQUE (user_id);


--
-- Name: user user_pkey; Type: CONSTRAINT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY user
    ADD CONSTRAINT user_pkey PRIMARY KEY (user_id);


--
-- Name: instruction instruction_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY instruction
    ADD CONSTRAINT instruction_user_id_fkey FOREIGN KEY (user_id) REFERENCES user(user_id);


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--

