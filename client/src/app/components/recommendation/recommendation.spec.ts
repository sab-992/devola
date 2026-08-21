import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RecommendationPage } from './recommendation';

describe('RecommendationPage', () => {
	let component: RecommendationPage;
	let fixture: ComponentFixture<RecommendationPage>;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [RecommendationPage],
		}).compileComponents();

		fixture = TestBed.createComponent(RecommendationPage);
		component = fixture.componentInstance;
		await fixture.whenStable();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
