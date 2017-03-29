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


--
-- Name: user; Type: TABLE; Schema: public; Owner: gbowner
--

CREATE TABLE "user" (
    user_id integer NOT NULL,
    identifier text NOT NULL,
    pixel_id integer
);

--
-- Name: users_id_seq; Type: SEQUENCE; Schema: public; Owner: gbowner
--

CREATE SEQUENCE users_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

--
-- Name: users_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gbowner
--

ALTER SEQUENCE users_id_seq OWNED BY "user".user_id;


--
-- Name: user user_id; Type: DEFAULT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY "user" ALTER COLUMN user_id SET DEFAULT nextval('users_id_seq'::regclass);

--
-- Name: users_id_seq; Type: SEQUENCE SET; Schema: public; Owner: gbowner
--

SELECT pg_catalog.setval('users_id_seq', 1, true);


--
-- Name: instruction instruction_user_id_key; Type: CONSTRAINT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY instruction
    ADD CONSTRAINT instruction_user_id_key UNIQUE (user_id);


--
-- Name: user users_pkey; Type: CONSTRAINT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY "user"
    ADD CONSTRAINT users_pkey PRIMARY KEY (user_id);


--
-- Name: instruction instruction_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gbowner
--

ALTER TABLE ONLY instruction
    ADD CONSTRAINT instruction_user_id_fkey FOREIGN KEY (user_id) REFERENCES "user"(user_id);


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

