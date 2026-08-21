import { DecimalPipe } from '@angular/common';
import { Component, computed, DestroyRef, inject, OnInit, signal } from '@angular/core';
import { takeUntilDestroyed } from '@angular/core/rxjs-interop';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { RecommendationRun, RecommendationResult } from '@models/recommendation';
import { RecommendationService } from '@services/recommendation/recommendation';
import { ThemeService } from '@services/theme/theme';
import { TimeService } from '@services/time/time';
import { HeaderComponent } from "@components/header/header";


@Component({
    selector: 'app-recommendations',
    standalone: true,
    imports: [DecimalPipe, MatButtonModule, MatIconModule, MatProgressSpinnerModule, HeaderComponent],
    templateUrl: './recommendation.html',
    styleUrl: './recommendation.scss',
    host: {
        '[attr.data-theme]': 'theme()',
    },
})
export class RecommendationPage implements OnInit {
    private readonly m_recommendationService = inject(RecommendationService);
    private readonly m_destroyRef = inject(DestroyRef);
    private readonly m_themeService = inject(ThemeService);

    readonly runs = signal<RecommendationRun[]>([]);
    readonly loadingRuns = signal(true);
    readonly runsError = signal<string | null>(null);
    readonly selectedUuid = signal<string | null>(null);
    readonly results = signal<RecommendationResult[]>([]);
    readonly loadingResults = signal(false);
    readonly resultsError = signal<string | null>(null);

    readonly starting = signal(false);
    readonly sortedRuns = computed(() => [...this.runs()].sort((a, b) => b.started_at - a.started_at));
    readonly selectedRun = computed(() => this.sortedRuns().find((run) => run.uuid === this.selectedUuid()) ?? null);
    readonly sortedResults = computed(() => [...this.results()].sort((a, b) => this.topScore(b) - this.topScore(a)));

    constructor() {}

    public ngOnInit(): void {
        this.loadRuns();
    }

    public get theme() {
        return this.m_themeService.theme;
    }

    public retryRuns(): void {
        this.loadRuns();
    }

    public selectRun(run: RecommendationRun): void {
        if (this.selectedUuid() === run.uuid) return;

        this.selectedUuid.set(run.uuid);
        this.results.set([]);
        this.resultsError.set(null);

        if (run.status !== 'completed') return;

        this.loadingResults.set(true);
        this.m_recommendationService
            .fetchResult(run.uuid)
            .pipe(takeUntilDestroyed(this.m_destroyRef))
            .subscribe({
                next: (results) => {
                    this.results.set(results);
                    this.loadingResults.set(false);
                },
                error: () => {
                    this.resultsError.set('Could not load results for this run.');
                    this.loadingResults.set(false);
                },
            });
    }

    public startRun(): void {
        if (this.starting())
            return;

        this.starting.set(true);
        this.m_recommendationService
            .startRecommendation()
            .pipe(takeUntilDestroyed(this.m_destroyRef))
            .subscribe({
                next: (run: RecommendationRun) => {
                    this.runs.update((list) => [run, ...list]);
                    this.starting.set(false);
                },
                error: () => {
                    this.starting.set(false);
                },
            });
    }

    public formatStatus(status: string): string {
        return status.charAt(0).toUpperCase() + status.slice(1);
    }

    public scoreWidth(score: number, result: RecommendationResult): number {
        const top = this.topScore(result);
        return top > 0 ? Math.round((score / top) * 100) : 0;
    }

    public timeAgo(timestampSeconds: number): string {
        return TimeService.timeAgo(timestampSeconds);
    }

    private loadRuns(): void {
        this.loadingRuns.set(true);
        this.runsError.set(null);

        this.m_recommendationService
            .fetchRuns()
            .pipe(takeUntilDestroyed(this.m_destroyRef))
            .subscribe({
                next: (runs: RecommendationRun[]) => {
                    this.runs.set(runs === null ? [] : runs);
                    this.loadingRuns.set(false);
                },
                error: () => {
                    this.runsError.set('Could not load recommendation runs.');
                    this.loadingRuns.set(false);
                },
            });
    }

    private topScore(result: RecommendationResult): number {
        return result.scores.reduce((max, s) => Math.max(max, s.score), 0);
    }
}