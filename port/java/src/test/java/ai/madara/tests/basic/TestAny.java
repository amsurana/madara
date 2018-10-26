/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 * 
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 * This material has been approved for public release and unlimited
 * distribution.
 * 
 * @author David Kyle <david.kyle@shield.ai>
 *********************************************************************/

package ai.madara.tests.basic;

import static org.junit.Assert.*;

import org.capnproto.MessageBuilder;
import org.capnproto.StructList.Builder;
import org.junit.Test;

import ai.madara.knowledge.Any;
import ai.madara.knowledge.BadAnyAccess;
import ai.madara.tests.BaseTest;
import ai.madara.tests.capnp.Geo;
import ai.madara.tests.capnp.Person;
import ai.madara.tests.capnp.Person.Bio.PhoneNumber.Type;
import ai.madara.tests.capnp.Person.Bio.Reader;

/**
 * This class is a tester for Any and AnyRef
 */
public class TestAny extends BaseTest {
	/**
	 * All the Any types are registered in {@link BaseTest#registerAnyTypes()}
	 * 
	 * Test all primitive data types (char, long, int, float, double) and String.
	 * 
	 * @throws InterruptedException
	 * @throws Exception
	 */
	@Test
	public void testPrimitiveDataTypes() throws InterruptedException, Exception {

		// System.err.println("Loading libdatatypes_shared.so");
		// System.loadLibrary("datatypes_shared");

		Any anyChar = new Any("char");
		anyChar.assign('A');
		assertEquals("A", anyChar.toString());

		Any anyLong = new Any("long");
		anyLong.assign(Long.MAX_VALUE);
		assertEquals(Long.MAX_VALUE, anyLong.toInteger());

		Any anyInt = new Any("int");
		anyInt.assign(Integer.MIN_VALUE);
		assertEquals(Integer.MIN_VALUE, anyInt.toInteger());
		assertEquals("" + Integer.MIN_VALUE, anyInt.toString());

		Any anyFloat = new Any("float");
		anyFloat.assign(123f);
		assertEquals(123f, anyFloat.toDouble(), 0.0);

		Any anyDouble = new Any("double");
		anyDouble.assign(3.14159);
		assertEquals((double) 22 / 7, anyDouble.toDouble(), 0.01);

		Any anyStr = new Any("string");
		anyStr.assign("Jungle Book");
		assertEquals("Jungle Book", anyStr.toString());

	}

	/**
	 * 
	 * @throws BadAnyAccess
	 */
	@Test
	public void testVectorTypes() throws BadAnyAccess {

		// Test String vector
		Any strVectorAny = new Any("strvec");
		strVectorAny.at(5).assign("fifth");
		strVectorAny.at(7).assign("seventh");
		strVectorAny.at(2).assign("second");

		assertFalse(strVectorAny.at(0).empty());
		assertNotNull(strVectorAny.at(2));
		assertEquals(strVectorAny.at(2).toString(), "second");

		// Test double vector
		Any dblVectorAny = new Any("dblvec");
		dblVectorAny.at(3).assign(3.1415);
		dblVectorAny.at(1).assign(1);
		dblVectorAny.at(100).assign(100);

		assertEquals((double) 22 / 7, dblVectorAny.at(3).toDouble(), 0.01);
		assertEquals(3, dblVectorAny.at(3).toInteger());
		assertEquals(1.0000, dblVectorAny.at(1).toDouble(), 0.0);
		assertEquals(100, dblVectorAny.at(100).toInteger());

		// Test Long vector
		Any longVectorAny = new Any("longvec");
		longVectorAny.at(1).assign(Long.MAX_VALUE);
		longVectorAny.at(20).assign(Long.MIN_VALUE);

		assertEquals(Long.MAX_VALUE, longVectorAny.at(1).toInteger());
		assertEquals(Long.MIN_VALUE, longVectorAny.at(20).toInteger());

		// Test int vector
		Any intVectorAny = new Any("intvec");
		intVectorAny.at(1).assign(Integer.MAX_VALUE);
		intVectorAny.at(2).assign(Integer.MIN_VALUE);

		assertEquals(Integer.MAX_VALUE, intVectorAny.at(1).toInteger());
		assertEquals(Integer.MIN_VALUE, intVectorAny.at(2).toInteger());

		// Test int vector
		Any shVectorAny = new Any("shvec");
		shVectorAny.at(1).assign(Short.MAX_VALUE);
		shVectorAny.at(2).assign(Short.MIN_VALUE);

		assertEquals(Short.MAX_VALUE, shVectorAny.at(1).toInteger());
		assertEquals(Short.MIN_VALUE, shVectorAny.at(2).toInteger());

	}

	public void testMapTypes() throws BadAnyAccess {
		Any a0 = new Any("smap");
		a0.at("hello").assign("world");

		assertEquals(a0.at("hello").toString(), "world");

	}

	@Test
	public void testSimpleObjectType() throws BadAnyAccess {
		Any c0 = getGeoPoint();

		Geo.Point.Reader reader = c0.reader(Geo.Point.factory);
		assertEquals(reader.getX(), DEFAULT_GEO_POINT[0], 0.0);
		assertEquals(reader.getY(), DEFAULT_GEO_POINT[1], 0.01);
		assertEquals(reader.getZ(), DEFAULT_GEO_POINT[2], 0);
	}

	@Test
	public void testComplexObjectType() throws BadAnyAccess {

		MessageBuilder bioMb = new MessageBuilder();
		Person.Bio.Builder bioBuilder = bioMb.initRoot(Person.Bio.factory);
		ai.madara.tests.capnp.Person.Date.Builder dobBuilder = bioBuilder.initBirthdate();
		dobBuilder.setDay((byte) 29);
		dobBuilder.setMonth((byte) 03);
		dobBuilder.setYear((short) 1986);

		bioBuilder.setEmail("amit@hakoonamatata.com");
		bioBuilder.setName("Amit S");

		Builder<ai.madara.tests.capnp.Person.Bio.PhoneNumber.Builder> phones = bioBuilder.initPhones(3);

		ai.madara.tests.capnp.Person.Bio.PhoneNumber.Builder homePhoneBuilder = phones.get(0);
		homePhoneBuilder.setNumber("+1 9900-123-123");
		homePhoneBuilder.setType(Type.HOME);

		bioBuilder.setBirthdate(dobBuilder.asReader());

		// Convert Person object to Any type.
		Any person = new Any(Person.Bio.factory, bioMb);

		// Read from Any type again.
		Reader obj = person.reader(Person.Bio.factory);

		// Assertion
		assertEquals(obj.getName().toString(), "Amit S");
	}

}
